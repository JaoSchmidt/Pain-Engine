/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Asteroid.cpp
#include "Asteroid.h"
#include <pain.h>

reg::Entity Asteroid::create(pain::Scene &scene, pain::TextureSheet &texSheet,
                             unsigned short id, glm::vec2 pos, glm::vec2 vel,
                             const float radius)
{
  // assuming size of quad sprite = diameter
  const glm::vec2 diameter = glm::vec2(2.f * radius);
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform2dComponent{pos},                           //
      pain::Movement2dComponent{vel},                                    //
      pain::RotationComponent{},                                         //
      pain::SpriteComponent::create({.m_size = diameter}, texSheet, id), //
      pain::SpritelessComponent::createCircle(radius),                   //
      pain::SAPCollider::createCircle(radius));
  return entity;
}
reg::Entity Asteroid::create(pain::Scene &scene, pain::TextureSheet &texSheet,
                             unsigned short id, glm::vec2 pos, glm::vec2 vel,
                             glm::vec2 size)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform2dComponent{pos},                       //
      pain::Movement2dComponent{vel},                                //
      pain::RotationComponent{},                                     //
      pain::SpriteComponent::create({.m_size = size}, texSheet, id), //
      pain::SpritelessComponent::createQuad(size),                   //
      pain::SAPCollider::createAABB(size));
  return entity;
}
