/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Wall.h"
#include <pain.h>

namespace Wall
{

reg::Entity create(pain::Scene &scene, const glm::vec2 &pos,
                   const glm::vec2 &size)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity, pain::Transform2dComponent{pos}, //
                         pain::SpritelessComponent::createQuad(
                             size, pain::Color{153, 153, 153, 102}), //
                         pain::SAPCollider::createAABB(size));
  return entity;
}
} // namespace Wall
