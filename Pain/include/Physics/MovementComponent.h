/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "pch.h"

namespace pain
{
struct Transform2dComponent {
  using tag = tag::Transform2d;
  glm::vec2 m_position{0.0f, 0.0f};
  static Transform2dComponent create() { return {}; }
  static Transform2dComponent create(const glm::vec2 &position)
  {
    return Transform2dComponent{.m_position = position};
  }
  operator glm::vec2 &() { return m_position; }
  operator const glm::vec2 &() const { return m_position; }
  Transform2dComponent clone() const { return *this; }
};

struct Movement2dComponent {
  using tag = tag::Movement2d;
  glm::vec2 m_velocity{0.0f, 0.0f};
  float m_rotationSpeed{0.0f};
  static Movement2dComponent create() { return {}; }
  static Movement2dComponent create(const glm::vec2 &velocity)
  {
    return Movement2dComponent{.m_velocity = velocity};
  }
  static Movement2dComponent create(const glm::vec2 &velocity,
                                    float rotationSpeed)
  {
    return Movement2dComponent{.m_velocity = velocity,
                               .m_rotationSpeed = rotationSpeed};
  }
};

} // namespace pain
