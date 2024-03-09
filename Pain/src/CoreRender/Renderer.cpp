#include "Renderer.h"

namespace pain
{

void Renderer::drawIndexed(const std::shared_ptr<VertexArray> &vertexArray)
{
  glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(),
                 GL_UNSIGNED_INT, nullptr);
}

void Renderer::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void setClearColor(const glm::vec4 color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::beginScene() {}

void Renderer::endScene() {}

void Renderer::submit(const std::shared_ptr<VertexArray> &vertexArray)
{
  vertexArray->bind();
  drawIndexed(vertexArray);
}

} // namespace pain
