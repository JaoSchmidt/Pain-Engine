#pragma once
#include "pch.h"

#include "Core.h"

#include "CoreRender/Camera.h"
#include "CoreRender/Shader.h"
#include "CoreRender/VertexArray.h"
#include "Misc/OrthoCameraController.h"

namespace pain
{

class EXPORT Renderer2d
{
public:
  static void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);
  static void beginScene(const glm::mat4 &transform = glm::mat4(1.0f));
  static void endScene();
  static void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                       const glm::vec4 &color);

  static void
  init(std::shared_ptr<OrthographicCameraController> &cameraController);
  static void shutdown();
  static void setViewport(int x, int y, int width, int height);
  static void setClearColor(const glm::vec4 color);
  static void clear();
  static void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray,
                          uint32_t indexCount = 0);

  static std::shared_ptr<OrthographicCameraController> m_cameraController;

private:
  static void flush();
};

} // namespace pain
