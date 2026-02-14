/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// PointLightBatch.cpp
#pragma once
#include "CoreRender/Shader.h"
#include <vector>

namespace pain
{

class PointLightBatch
{
public:
  static constexpr uint32_t MaxLights = 32;

  static PointLightBatch create();

  void reset();
  void addLight(const glm::vec3 &position, const glm::vec3 &color,
                float radius);

  void upload(Shader &shader);

private:
  std::vector<glm::vec4> m_positions; // xyz + radius
  std::vector<glm::vec4> m_colors;    // rgb + unused
};

} // namespace pain
