/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreFiles/RenderPipeline.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/Renderer/RenderApi.h"
#include "Misc/Events.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/MovementComponent.h"
#include "platform/ContextBackend.h"

#include <cstdlib>

// NOTE: technically this pipeline is incomplete because it doesn't take into
// account multiple screens

namespace pain
{
namespace
{
constexpr std::array<glm::vec4, 3> s_colorOptions = {
    glm::vec4(0.0F, 0.0F, 0.0F, 1.0F), // Pure Black
    glm::vec4(0.2F, 0.2F, 0.2F, 1.0F), // Dark Grey
    glm::vec4(1.0F, 0.2F, 0.9F, 1.0F)  // Strong Pink
};
constexpr glm::vec4 s_clearColor = s_colorOptions[1];
} // namespace

RenderPipeline::RenderPipeline(FrameBuffer frameBuffer,
                               reg::EventDispatcher &eventDispatcher)
    : m_frameBuffer(std::move(frameBuffer)),
      m_eventDispatcher(eventDispatcher) {};

RenderPipeline RenderPipeline::create(reg::EventDispatcher &eventDispatcher)

{
  return RenderPipeline::create(
      FrameBufferCreationInfo{.swapChainTarget = true}, eventDispatcher);
}

RenderPipeline RenderPipeline::create(const FrameBufferCreationInfo &info,
                                      reg::EventDispatcher &eventDispatcher)
{
  std::optional<FrameBuffer> fb = FrameBuffer::create(info);
  if (!fb.has_value()) {
    PLOG_E("FrameBuffer wasn't able to be created!");
    std::exit(1);
  }

  backend::setClearColor(s_clearColor);
  return RenderPipeline{std::move(*fb), eventDispatcher};
}

template <typename Camera>
  requires std::same_as<Camera, PerspCameraComponent> ||
           std::same_as<Camera, OrthoCameraComponent>
void resizeCamera(const SDL_Event &event, Camera &c, FrameBuffer &fb,
                  RenderApi &renderAPI)
{
  if (fb.getSpecification().swapChainTarget) {
    renderAPI.setViewPort(0, 0, event.window.data1, event.window.data2);
    c.setProjection(event.window.data1, event.window.data2);
  } else {
    // TODO: for minimaps/split screens bc (0,0,w,h) viewport won't work
    c.setProjection(fb.getWidthi(), fb.getHeighti());
    fb.resizeFrameBuffer(fb.getWidthi(), fb.getHeighti());
  }
}

namespace detail
{

void _implResizeFBViewport(
    const ImGuiViewportChangeEvent &event,
    std::vector<reg::ChunkView<pain::OrthoCameraComponent>> chunks,
    FrameBuffer &frameBuffer)
{
  const int newx = static_cast<int>(event.newSize.x);
  const int newy = static_cast<int>(event.newSize.y);
  frameBuffer.resizeFrameBuffer(newx, newy);
  for (auto &chunk : chunks) {
    auto *c = std::get<0>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i) {
      c[i].setProjection(newx, newy);
    }
  }
}

void _implResizeFBViewport(
    const ImGuiViewportChangeEvent &event,
    std::vector<reg::ChunkView<pain::PerspCameraComponent>> chunks,
    FrameBuffer &frameBuffer)
{
  const int newx = static_cast<int>(event.newSize.x);
  const int newy = static_cast<int>(event.newSize.y);
  frameBuffer.resizeFrameBuffer(newx, newy);
  for (auto &chunk : chunks) {
    auto *c = std::get<0>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i) {
      c[i].setProjection(newx, newy);
    }
  }
}

void _implChangeActiveCameraEvent(
    std::vector<reg::ChunkView<pain::OrthoCameraComponent>> chunks,
    pain::OrthoCameraComponent &cam, RenderApi &renderAPI)
{
  for (auto &chunk : chunks) {
    auto *c = std::get<0>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i)
      c[i].m_active = false;
  }
  renderAPI.setViewPort(cam.m_screenPosition.x, cam.m_screenPosition.y,
                        cam.m_resolution.x, cam.m_resolution.y);
}

void _implOnWindowResized(
    const SDL_Event &event, RenderApi &renderer, FrameBuffer &frameBuffer,
    std::vector<reg::ChunkView<pain::OrthoCameraComponent>> ocChunks,
    std::vector<reg::ChunkView<pain::PerspCameraComponent>> pcChunks)
{
  for (auto &chunk : ocChunks) {
    auto *c = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          resizeCamera(event, c[i], frameBuffer, renderer);
        }
      }
    }
  }
  for (auto &chunk : pcChunks) {
    auto *c = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          resizeCamera(event, c[i], frameBuffer, renderer);
        }
      }
    }
  }
}

std::optional<std::pair<const std::reference_wrapper<OrthoCameraComponent>,
                        const std::reference_wrapper<Transform2dComponent>>>
_implRetrieve2dCamera(
    std::vector<
        reg::ChunkView<pain::OrthoCameraComponent, pain::Transform2dComponent>>
        chunks)
{
  for (auto &chunk : chunks) {
    auto *c = std::get<0>(chunk.arrays);
    auto *t = std::get<1>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i) {
      if (c[i].m_active) {
        return std::pair(std::ref(c[i]), std::ref(t[i]));
      }
    }
  }
  return {};
}
std::optional<std::pair<const std::reference_wrapper<PerspCameraComponent>,
                        const std::reference_wrapper<Transform3dComponent>>>
_implRetrieve3dCamera(
    std::vector<
        reg::ChunkView<pain::PerspCameraComponent, pain::Transform3dComponent>>
        chunks)
{
  // TODO: implement for 3D OrthoCameras?
  for (auto &chunk : chunks) {
    auto *c = std::get<0>(chunk.arrays);
    auto *t = std::get<1>(chunk.arrays);
    for (size_t i = 0; i < chunk.count; ++i) {
      if (c[i].m_active) {
        return std::pair(std::ref(c[i]), std::ref(t[i]));
      }
    }
  }
  return {};
}

} // namespace detail

} // namespace pain
