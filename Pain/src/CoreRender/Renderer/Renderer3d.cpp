/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/Renderer3d.h"
#include "Assets/ManagerTexture.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Texture.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

#include "Debugging/Profiling.h"
#include "ECS/WorldScene.h"
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
bool Renderer3d::hasCamera() { return m.cameraEntity != reg::Entity{-1}; }
void Renderer3d::changeCamera(reg::Entity cameraEntity)
{
  m.cameraEntity = cameraEntity;
}

void Renderer3d::beginScene(DeltaTime globalTime, const Scene &scene,
                            const glm::mat4 &transform)
{
  PROFILE_FUNCTION();
  const Transform3dComponent &tc =
      std::as_const(scene).getComponent<Transform3dComponent>(m.cameraEntity);

  if (scene.hasAnyComponents<cmp::PerspCamera>(m.cameraEntity)) {
    const cmp::PerspCamera &c =
        std::as_const(scene).getComponent<Component::PerspCamera>(
            m.cameraEntity);
    uploadBasicUniforms(c.getViewProjectionMatrix(), globalTime, transform,
                        c.getResolution(), tc.m_position);
  } else if (scene.hasAnyComponents<cmp::OrthoCamera>(m.cameraEntity)) {
    const cmp::OrthoCamera &c =
        std::as_const(scene).getComponent<Component::OrthoCamera>(
            m.cameraEntity);
    uploadBasicUniforms(c.getViewProjectionMatrix(), globalTime, transform,
                        c.getResolution(), tc.m_position);
  } else {
    PLOG_E("The Renderer3d could't decide which camera you are using");
  }
  // Going back to frist vertex
  m.cubeBatch.resetAll();
  for (uint8_t i = 0; i < static_cast<uint8_t>(SphereDivision::Count); i++) {
    m.sphereBatches[i].resetAll();
  }
}

void Renderer3d::flush()
{
  PROFILE_FUNCTION();
  // bindTextures();
  m.cubeBatch.flush(m.textureSlots, m.textureSlotIndex);
  for (uint8_t i = 0; i < static_cast<uint8_t>(SphereDivision::Count); i++) {
    m.sphereBatches[i].flush(m.textureSlots, m.textureSlotIndex);
  }
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
  if (m.cubeBatch.indexCount >= CubeBatch::MaxIndices) {
    m.cubeBatch.flush(m.textureSlots, m.textureSlotIndex);
    m.cubeBatch.resetPtr();
  }
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
  if (m.cubeBatch.indexCount >= CubeBatch::MaxIndices) {
    m.cubeBatch.flush(m.textureSlots, m.textureSlotIndex);
    m.cubeBatch.resetPtr();
  }
  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size, rotation);
  m.cubeBatch.allocateCube(transform, color, tilingFactor, texIndex,
                           textureCoordinate);
}

void Renderer3d::drawUVSphere(const glm::vec3 &position, const glm::vec3 &size,
                              const Color &tintColor, Texture &texture,
                              SphereDivision div, float tilingFactor)
{
  PROFILE_FUNCTION();
  SphereBatch &batch = m.sphereBatches[static_cast<uint8_t>(div)];
  if (batch.indexCount >= batch.m_maxIndices) {
    batch.flush(m.textureSlots, m.textureSlotIndex);
    batch.resetPtr();
  }

  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size);
  batch.allocateSphereUV(transform, tintColor, tilingFactor, texIndex);
}

void Renderer3d::drawUVSphere(const glm::vec3 &position, const glm::vec3 &size,
                              const Color &tintColor, Texture &texture,
                              const glm::vec3 rotation, SphereDivision div,
                              float tilingFactor)
{
  PROFILE_FUNCTION();
  SphereBatch &batch = m.sphereBatches[static_cast<uint8_t>(div)];
  if (batch.indexCount >= batch.m_maxIndices) {
    batch.flush(m.textureSlots, m.textureSlotIndex);
    batch.resetPtr();
  }

  const float texIndex = allocateTextures(texture);
  const glm::mat4 transform = getTransform(position, size, rotation);
  batch.allocateSphereUV(transform, tintColor, tilingFactor, texIndex);
}

// ================================================================= //
// Transforms
// ================================================================= //

/// @brief Build a transform matrix without rotation.
glm::mat4 Renderer3d::getTransform(const glm::vec3 &position,
                                   const glm::vec3 &size)
{
  PROFILE_FUNCTION();
  return glm::translate(glm::mat4(1.0f), position) *
         glm::scale(glm::mat4(1.0f), size);
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

void Renderer3d::removeTexture(const Texture &texture)
{
  PROFILE_FUNCTION();
  if (texture.m_slot == 0) // m_textureSlots doesn't have the texture
    return;
  P_ASSERT_W(*m.textureSlots[texture.m_slot] == texture,
             "Attempted to remove a texture that wasn't allocated.");
  for (uint32_t i = texture.m_slot; i < m.textureSlotIndex - 1; i++) {
    m.textureSlots[i] = m.textureSlots[i + 1];
    m.textureSlots[i]->m_slot = i; // Update the slot value in the Texture
  }

  m.textureSlots[m.textureSlotIndex] = nullptr; // Clear last slot
  m.textureSlotIndex--;
  return;
}

void Renderer3d::uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                                     DeltaTime globalTime,
                                     const glm::mat4 &transform,
                                     const glm::ivec2 &resolution,
                                     const glm::vec2 &cameraPos)
{
  UNUSED(globalTime)
  UNUSED(resolution)
  UNUSED(cameraPos)
  PROFILE_FUNCTION();
  m.cubeBatch.shader.bind();
  m.cubeBatch.shader.uploadUniformMat4("u_ViewProjection",
                                       viewProjectionMatrix);
  m.cubeBatch.shader.uploadUniformMat4("u_Transform", transform);

  for (uint8_t i = 0; i < static_cast<uint8_t>(SphereDivision::Count); i++) {
    m.sphereBatches[i].shader.bind();
    m.sphereBatches[i].shader.uploadUniformMat4("u_ViewProjection",
                                                viewProjectionMatrix);
    m.sphereBatches[i].shader.uploadUniformMat4("u_Transform", transform);
  }
}
void Renderer3d::bindTextures()
{
  PROFILE_FUNCTION();
  for (uint32_t i = 0; i < m.textureSlotIndex; i++)
    m.textureSlots[i]->bindToSlot(i);
}
float Renderer3d::allocateTextures(Texture &texture)
{
  PROFILE_FUNCTION();
  float textureIndex = 0.0f;
  // use it to allocate new texture
  if (texture.m_slot == 0) {
    textureIndex = (float)m.textureSlotIndex;
    m.textureSlots[m.textureSlotIndex] = &texture;
    texture.m_slot = m.textureSlotIndex;
    m.textureSlotIndex++;
  } else {
    textureIndex = (float)texture.m_slot;
  }
  // TODO: check if m_textureSlotIndex is bigger than 32, then flush

  P_ASSERT_W(textureIndex != 0.0f,
             "Missing texture inside a drawQuad that requires textures");
  return textureIndex;
}
Renderer3d Renderer3d::createRenderer3d()
{
  PROFILE_FUNCTION();

  // First texture is a  1x1 white texture
  Texture **textureSlots = new Texture *[backend::getTMU()];
  textureSlots[0] =
      &TextureManager::getDefaultTexture(TextureManager::DefaultTexture::Blank);
  return Renderer3d([textureSlots] {
    return M{.sphereBatches = {SphereBatch::create(TP_VEC2(
                                   getResolution(SphereDivision::D_8x8))),
                               SphereBatch::create(TP_VEC2(
                                   getResolution(SphereDivision::D_16x16))),
                               SphereBatch::create(TP_VEC2(
                                   getResolution(SphereDivision::D_32x32)))},
             .cubeBatch = CubeBatch::create(),
             .textureSlots = textureSlots};
  }); //
}

} // namespace pain
