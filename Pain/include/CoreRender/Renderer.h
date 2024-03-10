#pragma once
#include "pch.gch"

#include "Core.h"

#include "Camera.h"
#include "Shader.h"
#include "VertexArray.h"

namespace pain
{

class EXPORT Renderer
{
public:
  static void beginScene(std::shared_ptr<Camera> &Camera);
  static void endScene();
  static void submit(const std::shared_ptr<Shader> shader,
                     const std::shared_ptr<VertexArray> &vertexArray);

  static void setClearColor(const glm::vec4 color);
  static void clear();
  static void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray);

  static std::shared_ptr<Camera> m_camera;
};

} // namespace pain
