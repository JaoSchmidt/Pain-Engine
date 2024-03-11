#pragma once
#include "pch.gch"

#include "Core.h"

#include "CoreRender/Camera.h"
#include "CoreRender/Shader.h"
#include "CoreRender/VertexArray.h"

namespace pain
{

class EXPORT Renderer
{
public:
  static void beginScene(std::shared_ptr<Camera> &Camera);
  static void endScene(const std::shared_ptr<VertexArray> &vertexArray);
  static void submit(const std::shared_ptr<Shader> shader,
                     const std::shared_ptr<VertexArray> &vertexArray,
                     const glm::mat4 &transform = glm::mat4(1.0f));

  static void setClearColor(const glm::vec4 color);
  static void clear();
  static void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray);

  static std::shared_ptr<Camera> m_camera;
};

} // namespace pain
