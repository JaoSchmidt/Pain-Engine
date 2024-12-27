#pragma once
#include "pch.h"

#include "Core.h"

#include "CoreRender/Camera.h"
#include "CoreRender/Renderer/CubeVertex.h"
#include "CoreRender/Shader.h"
#include "CoreRender/VertexArray.h"
#include "Misc/PerspCameraController.h"

namespace pain
{

class  Renderer3d
{
public:
  static void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);
  static void beginScene(const glm::mat4 &transform = glm::mat4(1.0f));
  static void endScene();
  static void drawCube(const glm::vec3 &position, const glm::vec3 &size,
                       const glm::vec4 &color);
  static void drawSimpleCube();

  static void
  init(std::shared_ptr<PerspectiveCameraController> &cameraController);
  static void shutdown();
  static void setViewport(int x, int y, int width, int height);
  static void setClearColor(const glm::vec4 color);
  static void clear();
  static void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray,
                          uint32_t indexCount = 0);

  static std::shared_ptr<PerspectiveCameraController> m_cameraController;

private:
  static CubeVertexBatch *m_cubeBatch;
  static void flush();
};

} // namespace pain
