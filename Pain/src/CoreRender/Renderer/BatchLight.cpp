/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// PointLightBatch.cpp

#include "CoreRender/Renderer/BatchLight.h"

namespace pain
{

PointLightBatch PointLightBatch::create()
{
  PointLightBatch batch;
  batch.m_positions.reserve(MaxLights);
  batch.m_colors.reserve(MaxLights);
  return batch;
}

void PointLightBatch::reset()
{
  m_positions.clear();
  m_colors.clear();
}

void PointLightBatch::addLight(const glm::vec3 &position,
                               const glm::vec3 &color, float radius)
{
  if (m_positions.size() >= MaxLights)
    return;

  m_positions.emplace_back(position, radius);
  m_colors.emplace_back(color, 1.0f);
}

void PointLightBatch::upload(Shader &shader)
{
  shader.bind();

  uint32_t count = static_cast<uint32_t>(m_positions.size());
  shader.uploadUniformInt("u_PointLightCount", count);

  for (uint32_t i = 0; i < count; i++) {
    shader.uploadUniformFloat4(
        "u_PointLights[" + std::to_string(i) + "].position", m_positions[i]);

    shader.uploadUniformFloat4("u_PointLights[" + std::to_string(i) + "].color",
                               m_colors[i]);
  }
}

} // namespace pain
