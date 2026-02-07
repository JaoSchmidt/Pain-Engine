/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/Renderer3d.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Texture.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

#include "Debugging/Profiling.h"
#include "Physics/Movement3dComponent.h"
#include "platform/ContextBackend.h"

namespace pain
{

extern const Texture *m_fontAtlasTexture;

// ================================================================= //
// Renderer: basic wrapper around opengl
// ================================================================= //

void Renderer3d::setViewport(int x, int y, int width, int height)
{
  backend::setViewPort(x, y, width, height);
}
void Renderer3d::clear() { backend::clear(); }

void Renderer3d::setClearColor(const glm::vec4 &color)
{
  backend::setClearColor(color);
}
bool Renderer3d::hasCamera() { return m.orthoCameraEntity != reg::Entity{-1}; }
void Renderer3d::changeCamera(reg::Entity cameraEntity)
{
  m.orthoCameraEntity = cameraEntity;
}

void Renderer3d::beginScene(DeltaTime globalTime, const Scene &scene,
                            const glm::mat4 &transform)
{
  PROFILE_FUNCTION();
  const cmp::PerspCamera &cc =
      std::as_const(scene).getComponent<Component::PerspCamera>(
          m.orthoCameraEntity);
  const Transform3dComponent &tc =
      std::as_const(scene).getComponent<Transform3dComponent>(
          m.orthoCameraEntity);
  uploadBasicUniforms(cc.getViewProjectionMatrix(), globalTime, transform,
                      cc.getResolution(), tc.m_position);
  goBackToFirstVertex();
}

void Renderer3d::flush()
{
  PROFILE_FUNCTION();
  // bindTextures();
  m.cubeBatch.flush(m.textureSlots, m.textureSlotIndex);
}

void Renderer3d::endScene()
{
  // quadBatch->sendAllDataToOpenGL();
  // NOTE: sendAllDataToOpenGL probably won't be here in the future,
  // otherwise flush() wouldn't need to be a function
  flush();
}

// ================================================================= //
// Draws
// ================================================================= //

void Renderer3d::drawCube(const glm::vec3 &position, const glm::vec3 &size,
                          const Color &color, Texture &texture,
                          float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size);
  m.cubeBatch.allocateCube(transform, color, tilingFactor, texIndex,
                           textureCoordinate);
}
void Renderer3d::drawCube(const glm::vec3 &position, const glm::vec3 &size,
                          const Color &color, const glm::vec3 rotation,
                          Texture &texture, float tilingFactor,
                          const std::array<glm::vec2, 4> &textureCoordinate)
{
  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size, rotation);
  m.cubeBatch.allocateCube(transform, color, tilingFactor, texIndex,
                           textureCoordinate);
}

// ================================================================= //
// Transforms
// ================================================================= //

/// @brief Build a transform matrix without rotation.
glm::mat4 Renderer3d::getTransform(const glm::vec3 &position,
                                   const glm::vec3 &size)
{
  PROFILE_FUNCTION();
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
  return glm::scale(transform, size);
}

/// @brief Build a transform matrix with rotation.
glm::mat4 Renderer3d::getTransform(const glm::vec3 &position,
                                   const glm::vec3 &size,
                                   const glm::vec3 &rotation)
{
  PROFILE_FUNCTION();
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::translate(transform, position);

  transform = glm::rotate(transform, rotation.x, {1.0f, 0.0f, 0.0f});
  transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
  transform = glm::rotate(transform, rotation.z, {0.0f, 0.0f, 1.0f});

  return glm::scale(transform, size);
}

} // namespace pain
