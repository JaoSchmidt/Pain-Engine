/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreRender/Renderer/Renderer3d.h"
#include "Assets/ManagerTexture.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Buffers/Texture.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Debugging/Profiling.h"
#include "ECS/WorldScene.h"
#include "Physics/Movement3dComponent.h"
#include "glm/gtc/quaternion.hpp"
#include "platform/ContextBackend.h"

namespace pain
{

// std::map<MaterialKey, CubeBatch> m_cubeBatchCache;
std::map<MaterialKey, SphereBatch> m_sphereBatchCache;
extern const Texture *m_fontAtlasTexture;

// ================================================================= //
// Renderer: basic wrapper around opengl
// ================================================================= //

bool Renderer3d::hasCamera() { return m.cameraEntity != reg::Entity{-1}; }
void Renderer3d::changeCamera(reg::Entity cameraEntity)
{
  m.cameraEntity = cameraEntity;
}

Renderer3d::Stats Renderer3d::getCubeStatistics()
{
  Stats cubeStats = {"Cubes"};
  for (auto it = m_cubeBatchCache.begin(); it != m_cubeBatchCache.end(); it++) {
    cubeStats += getStatistics(it->second);
  }
  return cubeStats;
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
    uploadBasicUniforms(scene, c.getViewProjectionMatrix(), globalTime,
                        transform, c.getResolution(), tc.m_position);
  } else if (scene.hasAnyComponents<cmp::OrthoCamera>(m.cameraEntity)) {
    const cmp::OrthoCamera &c =
        std::as_const(scene).getComponent<Component::OrthoCamera>(
            m.cameraEntity);
    uploadBasicUniforms(scene, c.getViewProjectionMatrix(), globalTime,
                        transform, c.getResolution(), tc.m_position);
  } else {
    PLOG_E("The Renderer3d could't decide which camera you are using");
  }
  // Going back to frist vertex
  // m.cubeBatch.resetAll();
  for (auto it = m_cubeBatchCache.begin(); it != m_cubeBatchCache.end(); it++) {
    it->second.resetAll();
  }
  for (auto it = m_sphereBatchCache.begin(); it != m_sphereBatchCache.end();
       it++) {
    it->second.resetAll();
  }
}

void Renderer3d::submitLight(const glm::vec3 &pos, const Color &color)
{
  for (auto it = m_sphereBatchCache.begin(); it != m_sphereBatchCache.end();
       it++) {
    if (std::holds_alternative<ParamPhong>(it->first.params)) {
      it->first.shader->bind();
      it->first.shader->uploadUniformFloat3("u_LightPos", pos);
      it->first.shader->uploadUniformFloat4("u_LightColor", color.getVector());
    }
  }
}

void Renderer3d::uploadBasicUniforms(const Scene &scene,
                                     const glm::mat4 &viewProjectionMatrix,
                                     DeltaTime globalTime,
                                     const glm::mat4 &transform,
                                     const glm::ivec2 &resolution,
                                     const glm::vec3 &cameraPos)
{
  UNUSED(scene);
  UNUSED(globalTime);
  UNUSED(resolution);
  UNUSED(transform);
  PROFILE_FUNCTION();

  for (auto it = m_cubeBatchCache.begin(); it != m_cubeBatchCache.end(); it++) {
    it->first.shader->bind();
    it->first.shader->uploadUniformMat4("u_ViewProjection",
                                        viewProjectionMatrix);
    // it->first.shader->uploadUniformMat4("u_Transform", transform);
  }
  for (auto it = m_sphereBatchCache.begin(); it != m_sphereBatchCache.end();
       it++) {
    it->first.shader->bind();
    it->first.shader->uploadUniformMat4("u_ViewProjection",
                                        viewProjectionMatrix);
    // it->first.shader->uploadUniformFloat3("u_ViewPos", cameraPos);
  }
  // m.cubeBatch.shader.bind();
  // m.cubeBatch.shader.uploadUniformMat4("u_ViewProjection",
  //                                      viewProjectionMatrix);
  // m.cubeBatch.shader.uploadUniformMat4("u_Transform", transform);
}

void Renderer3d::endScene(const Scene &scene)
{
  UNUSED(scene);
  // quadBatch->sendAllDataToOpenGL();
  // NOTE: sendAllDataToOpenGL probably won't be here in the future,
  // otherwise flush() wouldn't need to be a function
  flush();
}

void beforeFlush(const MaterialKey &mat)
{
  mat.shader->bind();
  // Upload instancing??
  // mat.shader->uploadUniformFloat4("u_Color", mat.color.getVector());
  // mat.shader->uploadUniformFloat("u_Tiling", mat.tiling);

  if (std::holds_alternative<ParamPBR>(mat.params)) {
    const ParamPBR &p = std::get<ParamPBR>(mat.params);
    mat.shader->uploadUniformFloat("u_Roughness", p.roughness);
    mat.shader->uploadUniformFloat("u_Metallic", p.metallic);
    mat.shader->uploadUniformFloat("u_Emission", p.emission);
  } else if (std::holds_alternative<ParamPhong>(mat.params)) {
    const ParamPhong &p = std::get<ParamPhong>(mat.params);
    mat.shader->uploadUniformFloat("u_Ambient", p.ambient);
    mat.shader->uploadUniformFloat("u_Diffuse", p.diffuse);
    mat.shader->uploadUniformFloat("u_Highlight", p.highlight);
  }
}

void Renderer3d::flush()
{
  PROFILE_FUNCTION();

  // bindTextures();
  for (auto it = m_cubeBatchCache.begin(); it != m_cubeBatchCache.end(); it++) {
    beforeFlush(it->first);
    it->second.flush(m.textureSlots, m.textureSlotIndex);
  }
  for (auto it = m_sphereBatchCache.begin(); it != m_sphereBatchCache.end();
       it++) {
    beforeFlush(it->first);
    it->second.flush(m.textureSlots, m.textureSlotIndex);
  }
  // m.cubeBatch.flush(m.textureSlots, m.textureSlotIndex,
  //                   m.materialManager.getDefaultShader(DefaultShader::Texture));
}

// ================================================================= //
// submits
// ================================================================= //

void Renderer3d::submitCube(const glm::vec3 &position, float size,
                            const Material &material)
{
  MaterialKey key{.shader = material.m_shader,
                  .params = material.m_params,
                  .flags = material.m_flags};
  auto it = m_cubeBatchCache.find(key);
  if (it == m_cubeBatchCache.end()) {
    auto [newIt, _] =
        m_cubeBatchCache.emplace(std::move(key), CubeBatch::create("batch"));
    it = newIt;
  }
  CubeBatch &batch = it->second;
  if (batch.m_count >= CubeBatch::MaxPolyhedrons) {
    beforeFlush(it->first);
    batch.flush(m.textureSlots, m.textureSlotIndex);
    batch.resetPtr();
  }

  const float texIndex = allocateTextures(*material.m_texture);
  glm::mat4 transform = getUniformScaleTransform(position, size);
  batch.allocateCube(transform, material.m_color, material.m_tilingFactor,
                     texIndex);
}

void Renderer3d::submitCube(const glm::vec3 &position, float size,
                            const Material &material, const glm::vec3 &rotation)
{
  MaterialKey key{.shader = material.m_shader,
                  .params = material.m_params,
                  .flags = material.m_flags};
  auto it = m_cubeBatchCache.find(key);
  if (it == m_cubeBatchCache.end()) {
    auto [newIt, _] =
        m_cubeBatchCache.emplace(std::move(key), CubeBatch::create("batch"));
    it = newIt;
  }
  CubeBatch &batch = it->second;

  if (batch.m_count >= CubeBatch::MaxPolyhedrons) {
    beforeFlush(it->first);
    batch.flush(m.textureSlots, m.textureSlotIndex);
    batch.resetPtr();
  }

  const float texIndex = allocateTextures(*material.m_texture);
  const glm::mat4 transform =
      getUniformScaleTransform(position, size, rotation);
  batch.allocateCube(transform, material.m_color, material.m_tilingFactor,
                     texIndex);
}

void Renderer3d::submitUVSphere(const glm::vec3 &position, float size,
                                SphereDivision div, const Material &material)
{
  PROFILE_FUNCTION();
  MaterialKey key{.shader = material.m_shader,
                  .params = material.m_params,
                  .flags = material.m_flags};
  auto it = m_sphereBatchCache.find(key);
  if (it == m_sphereBatchCache.end()) {
    auto [newIt, _] = m_sphereBatchCache.emplace(
        std::move(key),
        SphereBatch::create(TP_VEC2(getResolution(div)), "batch"));
    it = newIt;
  }
  SphereBatch &batch = it->second;

  if (batch.m_count >= CubeBatch::MaxPolyhedrons) {
    beforeFlush(it->first);
    batch.flush(m.textureSlots, m.textureSlotIndex);
    batch.resetPtr();
  }

  const float texIndex = allocateTextures(*material.m_texture);
  const glm::mat4 transform = getUniformScaleTransform(position, size);
  batch.allocateSphereUV(transform, material.m_color, material.m_tilingFactor,
                         texIndex);
}

void Renderer3d::submitUVSphere(const glm::vec3 &position, float size,
                                SphereDivision div, const Material &material,
                                const glm::vec3 &rotation)
{
  PROFILE_FUNCTION();
  MaterialKey key{.shader = material.m_shader,
                  .params = material.m_params,
                  .flags = material.m_flags};
  auto it = m_sphereBatchCache.find(key);
  if (it == m_sphereBatchCache.end()) {
    auto [newIt, _] = m_sphereBatchCache.emplace(
        std::move(key),
        SphereBatch::create(TP_VEC2(getResolution(div)), "batch"));
    it = newIt;
  }
  SphereBatch &batch = it->second;

  if (batch.m_count >= CubeBatch::MaxPolyhedrons) {
    beforeFlush(it->first);
    batch.flush(m.textureSlots, m.textureSlotIndex);
    batch.resetPtr();
  }

  const float texIndex = allocateTextures(*material.m_texture);
  const glm::mat4 transform =
      getUniformScaleTransform(position, size, rotation);
  batch.allocateSphereUV(transform, material.m_color, material.m_tilingFactor,
                         texIndex);
}

// ================================================================= //
// Transforms
// ================================================================= //

/// @brief Build a transform matrix without rotation.
glm::mat4 Renderer3d::getUniformScaleTransform(const glm::vec3 &position,
                                               float size)
{
  PROFILE_FUNCTION();
  return glm::translate(glm::mat4(1.0f), position) *
         glm::scale(glm::mat4(1.0f), glm::vec3(size));
}

/// @brief Build a transform matrix with rotation.
glm::mat4 Renderer3d::getUniformScaleTransform(const glm::vec3 &position,
                                               float size,
                                               const glm::vec3 &rotation)
{
  PROFILE_FUNCTION();
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::translate(transform, position);

  // yaw -> pitch -> roll in order
  transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
  transform = glm::rotate(transform, rotation.x, {1.0f, 0.0f, 0.0f});
  transform = glm::rotate(transform, rotation.z, {0.0f, 0.0f, 1.0f});

  return glm::scale(transform, glm::vec3(size));
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
             "Missing texture inside a submitQuad that requires textures");
  return textureIndex;
}
Renderer3d Renderer3d::createRenderer3d(MaterialManager &materialManager)
{
  PROFILE_FUNCTION();

  // First texture is a  1x1 white texture
  Texture **textureSlots = new Texture *[backend::getTMU()];
  textureSlots[0] =
      &TextureManager::getDefaultTexture(TextureManager::DefaultTexture::Blank);
  return Renderer3d([textureSlots, &materialManager] {
    return M{
        .materialManager = materialManager,           //
        .textureSlots = textureSlots,                 //
        .cubeBatch = CubeBatch::create("m.cubeBatch") //
    };
  }); //
}

} // namespace pain
