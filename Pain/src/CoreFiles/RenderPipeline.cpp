/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreFiles/RenderPipeline.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/Renderer/Renderers.h"
#include "ECS/UIScene.h"
#include "ECS/WorldScene.h"
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
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Pure Black
    glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), // Dark Grey
    glm::vec4(1.0f, 0.2f, 0.9f, 1.0f)  // Strong Pink
};
constexpr glm::vec4 s_clearColor = s_colorOptions[1];
} // namespace

template <typename Camera>
  requires std::same_as<Camera, cmp::PerspCamera> ||
           std::same_as<Camera, cmp::OrthoCamera>
void resizeFBViewport(const ImGuiViewportChangeEvent &event, Camera &cc,
                      FrameBuffer &frameBuffer)
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

void RenderPipeline::subscribeToEvents(Scene &scene, Renderers &renderers)
{
  m_eventDispatcher.subscribe<ImGuiViewportChangeEvent>(
      [&](const ImGuiViewportChangeEvent &e) {
        auto chunks = scene.query<cmp::OrthoCamera>();
        for (auto &chunk : chunks) {
          auto *c = std::get<0>(chunk.arrays);

          for (size_t i = 0; i < chunk.count; ++i) {
            resizeFBViewport(e, c[i], m_frameBuffer);
          }
        }
        auto chunks2 = scene.query<cmp::PerspCamera>();
        for (auto &chunk : chunks2) {
          auto *c = std::get<0>(chunk.arrays);

          for (size_t i = 0; i < chunk.count; ++i) {
            resizeFBViewport(e, c[i], m_frameBuffer);
          }
        }
      });
  m_eventDispatcher.subscribe<ChangeActiveCameraEvent>(
      [&](const ChangeActiveCameraEvent &e) {
        auto &cam = scene.getComponent<cmp::OrthoCamera>(e.cam);
        if (cam.m_active) {
          auto chunks = scene.query<cmp::OrthoCamera>();
          for (auto &chunk : chunks) {
            auto *c = std::get<0>(chunk.arrays);
            for (size_t i = 0; i < chunk.count; ++i)
              c[i].m_active = false;
          }
          renderers.setViewPort(cam.m_screenPosition.x, cam.m_screenPosition.y,
                                cam.m_resolution.x, cam.m_resolution.y);
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

  backend::setClearColor(s_clearColor);
  return RenderPipeline{std::move(*fb), eventDispatcher};
}

template <typename Camera>
  requires std::same_as<Camera, cmp::PerspCamera> ||
           std::same_as<Camera, cmp::OrthoCamera>
void resizeCamera(const SDL_Event &event, Camera &c, FrameBuffer &fb,
                  Renderers &renderers)
{
  if (fb.getSpecification().swapChainTarget) {
    renderers.setViewPort(0, 0, event.window.data1, event.window.data2);
    c.setProjection(event.window.data1, event.window.data2);
  } else {
    // TODO: for minimaps/split screens bc (0,0,w,h) viewport won't work
    c.setProjection(fb.getWidthi(), fb.getHeighti());
    fb.resizeFrameBuffer(fb.getWidthi(), fb.getHeighti());
  }
}

void RenderPipeline::onWindowResized(const SDL_Event &event,
                                     Renderers &renderer, Scene &scene)
{
  {
    auto chunks = scene.query<cmp::OrthoCamera>();
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
    auto chunks = scene.query<cmp::PerspCamera>();
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

std::optional<std::pair<const std::reference_wrapper<cmp::OrthoCamera>,
                        const std::reference_wrapper<Transform2dComponent>>>
retrieve2dCamera(Scene &scene)
{
  auto chunks = scene.query<cmp::OrthoCamera, Transform2dComponent>();
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
std::optional<std::pair<const std::reference_wrapper<cmp::PerspCamera>,
                        const std::reference_wrapper<Transform3dComponent>>>
retrieve3dCamera(Scene &scene)
{
  // TODO: implement for 3D OrthoCameras?
  auto chunks = scene.query<cmp::PerspCamera, Transform3dComponent>();
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

void RenderPipeline::pipeline(Renderers &renderers, bool isMinimized,
                              DeltaTime currentTime, Scene &worldScene,
                              UIScene &uiScene)
{
  m_frameBuffer.bind();
  backend::clear();
  auto wrap2d = retrieve2dCamera(worldScene);
  auto wrap3d = retrieve3dCamera(worldScene);

  // Scripts don't actually use renderers, they fill the render context
  worldScene.renderSystems(RenderPass::Script, renderers, isMinimized,
                           currentTime);

  P_ASSERT_W(wrap3d || wrap2d, "No active defualt camera");
  if (wrap3d) {
    backend::enable3d();
    renderers.m_renderer3d.beginScene(currentTime, wrap3d->first,
                                      wrap3d->second);
    worldScene.renderSystems(RenderPass::Dim3d, renderers, isMinimized,
                             currentTime);
    renderers.m_renderer3d.endScene(worldScene);
  }
  if (wrap2d) {
    backend::disable3d();
    renderers.m_renderer2d.beginScene(currentTime, wrap2d->first,
                                      wrap2d->second);
    worldScene.renderSystems(RenderPass::Dim2d, renderers, isMinimized,
                             currentTime);
    renderers.m_renderer2d.endScene();
  }

  renderers.m_renderContext.clear();

  m_frameBuffer.unbind();
  uiScene.renderSystems(RenderPass::UI, renderers, isMinimized, currentTime);
}

} // namespace pain
