#include "CoreRender/RenderContext.h"
#include "glm/ext/matrix_transform.hpp"
namespace pain
{

void RenderContext::submitCube(const glm::vec3 &position, float size,
                               const Material &material)
{
  m_commands.emplace_back(
      RenderCommandType::Cube,
      RenderCommand::Data{
          {&material, getUniformScaleTransform(position, size)} //
      } //
  );
}

void RenderContext::submitCube(const glm::vec3 &position, float size,
                               const Material &material,
                               const glm::vec3 &rotation)
{
  m_commands.emplace_back(
      RenderCommandType::Cube,
      RenderCommand::Data{
          {&material, getUniformScaleTransform(position, size, rotation)} //
      } //
  );
}
void RenderContext::submitUVSphere(const glm::vec3 &position, float size,
                                   SphereDivision div, const Material &material)
{
  m_commands.emplace_back(
      s_renderCommandTypes[static_cast<uint8_t>(div)],
      RenderCommand::Data{
          {&material, getUniformScaleTransform(position, size)} //
      } //
  );
}
void RenderContext::submitUVSphere(const glm::vec3 &position, float size,
                                   SphereDivision div, const Material &material,
                                   const glm::vec3 &rotation)
{
  m_commands.emplace_back(
      s_renderCommandTypes[static_cast<uint8_t>(div)],
      RenderCommand::Data{
          {&material, getUniformScaleTransform(position, size, rotation)} //
      } //
  );
}

void RenderContext::submitLight(const glm::vec3 &pos, const Color &color)
{
  m_commands.emplace_back( //
      RenderCommandType::LightPoint,
      RenderCommand::Data{
          .lightPoint{pos, color} //
      } //
  );
}

glm::mat4 RenderContext::getUniformScaleTransform(const glm::vec3 &position,
                                                  float size)
{
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::translate(transform, position);
  return glm::scale(transform, glm::vec3(size));
}
glm::mat4 RenderContext::getUniformScaleTransform(const glm::vec3 &position,
                                                  float size,
                                                  const glm::vec3 &rotation)
{
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::translate(transform, position);

  // yaw -> pitch -> roll in order
  transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
  transform = glm::rotate(transform, rotation.x, {1.0f, 0.0f, 0.0f});
  transform = glm::rotate(transform, rotation.z, {0.0f, 0.0f, 1.0f});

  return glm::scale(transform, glm::vec3(size));
}

RenderContext RenderContext::create(MaterialManager &materialManager)
{
  return RenderContext(materialManager);
}
RenderContext::RenderContext(MaterialManager &materialManager)
    : m_materialManager(materialManager) {};

} // namespace pain
