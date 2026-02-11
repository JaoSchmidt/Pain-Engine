/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreFiles/RenderPipeline.h"
#include "ContextBackend.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/Renderer/RenderContext.h"
#include "ECS/UIScene.h"
#include "ECS/WorldScene.h"
#include "Misc/Events.h"

#include <cstdlib>

// NOTE: technically this pipeline is incomplete because it doesn't take into
// account multiple screens

namespace pain
{
namespace
{
constexpr std::array<glm::vec4, 3> s_colorOptions = {
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Pure Black
    glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), // Dark Grey
    glm::vec4(1.0f, 0.2f, 0.9f, 1.0f)  // Strong Pink
};
constexpr glm::vec4 s_clearColor = s_colorOptions[1];
} // namespace

void resizeFBViewport(const ImGuiViewportChangeEvent &event,
                      Component::OrthoCamera &cc, FrameBuffer &frameBuffer)
{
  int newx = static_cast<int>(event.newSize.x);
  int newy = static_cast<int>(event.newSize.y);
  frameBuffer.resizeFrameBuffer(newx, newy);
  cc.setProjection(newx, newy);
}

RenderPipeline::RenderPipeline(FrameBuffer frameBuffer,
                               reg::EventDispatcher &eventDispatcher)
    : m_frameBuffer(std::move(frameBuffer)),
      m_eventDispatcher(eventDispatcher) {};

void RenderPipeline::subscribeToViewportChange(Scene &scene)
{
  m_eventDispatcher.subscribe<ImGuiViewportChangeEvent>(
      [&](const ImGuiViewportChangeEvent &e) {
        auto chunks = scene.query<Component::OrthoCamera>();
        for (auto &chunk : chunks) {
          auto *c = std::get<0>(chunk.arrays);

          for (size_t i = 0; i < chunk.count; ++i) {
            resizeFBViewport(e, c[i], m_frameBuffer);
          }
        }
      });
};

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

  return RenderPipeline{std::move(*fb), eventDispatcher};
}

template <typename Camera>
  requires std::same_as<Camera, cmp::PerspCamera> ||
           std::same_as<Camera, cmp::OrthoCamera>
void resizeCamera(const SDL_Event &event, Camera &c, FrameBuffer &fb,
                  Renderers &renderers)
{
  if (fb.getSpecification().swapChainTarget) {
    renderers.renderer2d.setViewport(0, 0, event.window.data1,
                                     event.window.data2);
    renderers.renderer3d.setViewport(0, 0, event.window.data1,
                                     event.window.data2);
    c.setProjection(event.window.data1, event.window.data2);
  } else {
    c.setProjection(fb.getWidthi(), fb.getHeighti());
    fb.resizeFrameBuffer(fb.getWidthi(), fb.getHeighti());
  }
}

void RenderPipeline::onWindowResized(const SDL_Event &event,
                                     Renderers &renderer, Scene &scene)
{
  {
    auto chunks = scene.query<Component::OrthoCamera>();
    for (auto &chunk : chunks) {
      auto *c = std::get<0>(chunk.arrays);

      for (size_t i = 0; i < chunk.count; ++i) {
        if (event.type == SDL_WINDOWEVENT) {
          if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            resizeCamera(event, c[i], m_frameBuffer, renderer);
          }
        }
      }
    }
  }
  {
    auto chunks = scene.query<Component::PerspCamera>();
    for (auto &chunk : chunks) {
      auto *c = std::get<0>(chunk.arrays);

      for (size_t i = 0; i < chunk.count; ++i) {
        if (event.type == SDL_WINDOWEVENT) {
          if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
            resizeCamera(event, c[i], m_frameBuffer, renderer);
          }
        }
      }
    }
  }
}

void RenderPipeline::pipeline(Renderers &renderers, bool isMinimized,
                              DeltaTime currentTime, Scene &worldScene,
                              UIScene &uiScene)
{
  backend::clear();
  backend::setClearColor(s_clearColor);
  m_frameBuffer.bind();
  if (renderers.renderer2d.hasCamera()) {
    renderers.renderer2d.beginScene(currentTime, worldScene);
    worldScene.renderSystems(renderers, isMinimized, currentTime);
    renderers.renderer2d.endScene();
  }
  if (renderers.renderer3d.hasCamera()) {
    renderers.renderer3d.beginScene(currentTime, worldScene);
    worldScene.renderSystems(renderers, isMinimized, currentTime);
    renderers.renderer3d.endScene();
  }
  m_frameBuffer.unbind();
  uiScene.renderSystems(renderers, isMinimized, currentTime);
}

} // namespace pain
