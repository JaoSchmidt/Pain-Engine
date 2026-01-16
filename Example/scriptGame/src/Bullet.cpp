/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Bullet.h"
#include "ECS/Components/Particle.h"
#include "Scripting/Component.h"
#include <pain.h>

reg::Entity Bullet::create(pain::Scene &scene)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity, pain::Transform2dComponent{}, //
                         pain::SpriteComponent{},              //
                         pain::Movement2dComponent{});
  return entity;
}
