/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Stars.h"
#include <pain.h>

namespace Stars
{
reg::Entity create(pain::Scene &scene, pain::TextureSheet &texSheet,
                   unsigned short id, glm::vec2 &pos)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity,
                         pain::Transform2dComponent{glm::vec3(pos, 0.f)}, //
                         pain::SpriteComponent::create({}, texSheet, id));
  return entity;
}
reg::Entity create(pain::Scene &scene, pain::Texture &tex, unsigned short id,
                   glm::vec2 &pos)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity,
                         pain::Transform2dComponent{glm::vec3(pos, 0.f)}, //
                         pain::SpriteComponent::create({}, tex));
  return entity;
}
} // namespace Stars
