#include "CoreRender/Renderer.h"

namespace pain
{
// std::shared_ptr<Camera> Renderer::m_camera = nullptr;
std::shared_ptr<OrthographicCameraController> Renderer::m_cameraController =
    nullptr;
void Renderer::drawIndexed(const std::shared_ptr<VertexArray> &vertexArray)
{
  glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(),
                 GL_UNSIGNED_INT, nullptr);
}

void Renderer::setViewport(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
}

void Renderer::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void Renderer::setClearColor(const glm::vec4 color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::beginScene(
    std::shared_ptr<OrthographicCameraController> &cameraController)
{
  m_cameraController = cameraController;
}

void Renderer::endScene(const std::shared_ptr<VertexArray> &vertexArray)
{
  vertexArray->bind();
  drawIndexed(vertexArray);
}

void Renderer::submit(const std::shared_ptr<Shader> shader,
                      const std::shared_ptr<VertexArray> &vertexArray,
                      const glm::mat4 &transform)
{
  shader->bind();
  shader->uploadUniformMat4(
      "u_ViewProjection",
      m_cameraController->getCamera().getViewProjectionMatrix());
  shader->uploadUniformMat4("u_Transform", transform);
}

} // namespace pain
