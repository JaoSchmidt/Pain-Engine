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
      RenderCommandType::Sphere,
      RenderCommand::Data{
          .sphere{&material, getUniformScaleTransform(position, size), div} //
      } //
  );
}
void RenderContext::submitUVSphere(const glm::vec3 &position, float size,
                                   SphereDivision div, const Material &material,
                                   const glm::vec3 &rotation)
{
  m_commands.emplace_back(
      RenderCommandType::Sphere,
      RenderCommand::Data{
          .sphere{&material, getUniformScaleTransform(position, size, rotation),
                  div} //
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

// ================================================================= //
// Submit 2d functions
// ================================================================= //

void RenderContext::submitQuad(const glm::vec2 &position, float size,
                               RenderLayer layer, const Material &material)
{
  m_commands.emplace_back(
      RenderCommandType::Quad,
      RenderCommand::Data{
          .sprite{&material, getUniformScaleTransform(position, size), layer} //
      } //
  );
}
void RenderContext::submitQuad(const glm::vec2 &position, float size,
                               const float rotationRadians, RenderLayer layer,
                               const Material &material)
{
  m_commands.emplace_back(
      RenderCommandType::Quad,
      RenderCommand::Data{
          .sprite{&material,
                  getUniformScaleTransform(position, size, rotationRadians),
                  layer} //
      } //
  );
}
void RenderContext::submitRect(const glm::vec2 &position, const glm::vec2 &size,
                               RenderLayer layer, const Material &material)
{
  m_commands.emplace_back(
      RenderCommandType::Rect,
      RenderCommand::Data{
          .sprite{&material, getTransform(position, size), layer} //
      } //
  );
}
void RenderContext::submitRect(const glm::vec2 &position, const glm::vec2 &size,
                               const float rotationRadians, RenderLayer layer,
                               const Material &material)
{
  m_commands.emplace_back(
      RenderCommandType::Rect,
      RenderCommand::Data{
          .sprite{&material, getTransform(position, size, rotationRadians),
                  layer} //
      } //
  );
}
void RenderContext::submitTri(const glm::vec2 &position, float size,
                              RenderLayer layer, const Material &material)
{
  m_commands.emplace_back(
      RenderCommandType::Triangle,
      RenderCommand::Data{
          .sprite{&material, getUniformScaleTransform(position, size), layer} //
      } //
  );
}
void RenderContext::submitTri(const glm::vec2 &position, float size,
                              const float rotationRadians, RenderLayer layer,
                              const Material &material)
{
  m_commands.emplace_back(
      RenderCommandType::Triangle,
      RenderCommand::Data{
          .sprite{&material,
                  getUniformScaleTransform(position, size, rotationRadians),
                  layer} //
      } //
  );
}

// =================================================================== //
// Private Transforms
// =================================================================== //

glm::mat4 RenderContext::getTransform(const glm::vec2 &position,
                                      const glm::vec2 &size,
                                      float rotationAngleRadians)
{
  return glm::translate(glm::mat4(1.0F), {position, 0.F}) *
         glm::rotate(glm::mat4(1.0F), rotationAngleRadians,
                     {0.0F, 0.0F, 1.0F}) *
         glm::scale(glm::mat4(1.0F), {size.x, size.y, 1.0F});
}
glm::mat4 RenderContext::getTransform(const glm::vec2 &position,
                                      const glm::vec2 &size)
{
  return glm::translate(glm::mat4(1.0F), {position, 0.F}) *
         glm::scale(glm::mat4(1.0F), {size.x, size.y, 1.0F});
}

glm::mat4 RenderContext::getUniformScaleTransform(const glm::vec2 &position,
                                                  float size,
                                                  float rotationAngleRadians)
{
  return glm::translate(glm::mat4(1.0F), {position, 0.F}) *
         glm::rotate(glm::mat4(1.0F), rotationAngleRadians,
                     {0.0F, 0.0F, 1.0F}) *
         glm::scale(glm::mat4(1.0F), {size, size, 1.0F});
}
glm::mat4 RenderContext::getUniformScaleTransform(const glm::vec2 &position,
                                                  float size)
{
  return glm::translate(glm::mat4(1.0F), {position, 0.F}) *
         glm::scale(glm::mat4(1.0F), {size, size, 1.0F});
}
glm::mat4 RenderContext::getUniformScaleTransform(const glm::vec3 &position,
                                                  float size)
{
  glm::mat4 transform = glm::mat4(1.0F);
  transform = glm::translate(transform, position);
  return glm::scale(transform, glm::vec3(size));
}
glm::mat4 RenderContext::getUniformScaleTransform(const glm::vec3 &position,
                                                  float size,
                                                  const glm::vec3 &rotation)
{
  glm::mat4 transform = glm::mat4(1.0F);
  transform = glm::translate(transform, position);

  // yaw -> pitch -> roll in order
  transform = glm::rotate(transform, rotation.y, {0.0F, 1.0F, 0.0F});
  transform = glm::rotate(transform, rotation.x, {1.0F, 0.0F, 0.0F});
  transform = glm::rotate(transform, rotation.z, {0.0F, 0.0F, 1.0F});

  return glm::scale(transform, glm::vec3(size));
}

RenderContext RenderContext::create(MaterialManager &materialManager)
{
  return RenderContext(materialManager);
}
RenderContext::RenderContext(MaterialManager &materialManager)
    : m_materialManager(materialManager) {};

} // namespace pain
