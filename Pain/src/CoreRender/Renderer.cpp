#include "Renderer.h"

namespace pain
{
std::shared_ptr<Camera> Renderer::m_camera = nullptr;
void Renderer::drawIndexed(const std::shared_ptr<VertexArray> &vertexArray)
{
  glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(),
                 GL_UNSIGNED_INT, nullptr);
}

void Renderer::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void Renderer::setClearColor(const glm::vec4 color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::beginScene(std::shared_ptr<Camera> &camera)
{
  m_camera = camera;
}

void Renderer::endScene(SDL_Window *m_window) { SDL_GL_SwapWindow(m_window); }

void Renderer::submit(const std::shared_ptr<Shader> shader,
                      const std::shared_ptr<VertexArray> &vertexArray)
{
  shader->bind();
  shader->uploadUniformMat4("u_ViewProjection",
                            m_camera->GetViewProjectionMatrix());
  vertexArray->bind();
  drawIndexed(vertexArray);
}

} // namespace pain
