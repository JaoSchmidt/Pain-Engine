#pragma once
#include "pch.gch"

#include "VertexArray.h"

namespace pain
{

class Renderer
{
public:
  static void beginScene();
  static void endScene();
  static void submit(const std::shared_ptr<VertexArray> &vertexArray);

  static void setClearColor(const glm::vec4 color);
  static void clear();
  static void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray);
};

} // namespace pain
