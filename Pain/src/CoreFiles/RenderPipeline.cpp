
#include "CoreFiles/RenderPipeline.h"

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
