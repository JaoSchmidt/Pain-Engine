
#include "CoreFiles/RenderPipeline.h"
#include "ECS/Components/Camera.h"
#include "ECS/Scene.h"

#include <cstdlib>

namespace pain
{
namespace
{
constexpr std::array<glm::vec4, 3> s_colorOptions = {
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), // Pure Black
    glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), // Dark Grey
    glm::vec4(1.0f, 0.2f, 0.9f, 1.0f)  // Strong Pink
};
constexpr glm::vec4 s_clearColor = s_colorOptions[2];
} // namespace

RenderPipeline::RenderPipeline(FrameBuffer frameBuffer)
    : m_frameBuffer(std::move(frameBuffer)) {};

RenderPipeline RenderPipeline::create()
{
  return RenderPipeline::create(
      FrameBufferCreationInfo{.swapChainTarget = true});
}

RenderPipeline RenderPipeline::create(const FrameBufferCreationInfo &info)
{
  std::optional<FrameBuffer> fb = FrameBuffer::createFrameBuffer(info);
  if (!fb.has_value()) {
    PLOG_E("FrameBuffer wasn't able to be created!");
    std::exit(1);
  }

  return RenderPipeline{std::move(*fb)};
}

void resizeCamera(const SDL_Event &event, OrthoCameraComponent &cc,
                  const FrameBuffer &fb)
{
  if (fb.getSpecification().swapChainTarget) {
    cc.m_matrices->setResolution(event.window.data1, event.window.data2);
    cc.m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  } else {
    cc.m_matrices->setResolution(static_cast<int>(fb.getWidth()),
                                 static_cast<int>(fb.getHeight()));
    cc.m_aspectRatio = fb.getWidthf() / fb.getHeightf();
  }
  cc.m_matrices->SetProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                               cc.m_aspectRatio * cc.m_zoomLevel,
                               -cc.m_zoomLevel, cc.m_zoomLevel);
}

void RenderPipeline::onWindowResized(const SDL_Event &event,
                                     Renderer2d &renderer, Scene &scene)
{
  renderer.setViewport(0, 0, event.window.data1, event.window.data2);
  auto chunks = scene.query<OrthoCameraComponent>();
  for (auto &chunk : chunks) {
    auto *__restrict c = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          resizeCamera(event, c[i], m_frameBuffer);
        }
      }
    }
  }
}

void RenderPipeline::pipeline(Renderer2d &renderer, bool isMinimized,
                              DeltaTime currentTime, Scene &worldScene)
{
  m_frameBuffer.bind();
  renderer.setClearColor(s_clearColor);
  renderer.clear();
  renderer.beginScene(currentTime, worldScene);
  worldScene.renderSystems(renderer, isMinimized, currentTime);
  renderer.endScene();
  m_frameBuffer.unbind();
  worldScene.renderImGui(renderer, isMinimized, currentTime);
}

} // namespace pain
