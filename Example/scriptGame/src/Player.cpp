/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Player.h"
#include "ECS/Components/Particle.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "Misc/Events.h"
#include "Physics/Collision/Collider.h"
#include "Scripting/Component.h"
#include <pain.h>

reg::Entity Player::create(pain::Scene &scene, pain::Texture &tex,
                           glm::vec2 initialPos, glm::vec2 size,
                           int resolutionHeight, int resolutionWeigh,
                           float zoomLevel)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(                             //
      entity, pain::Transform2dComponent{initialPos}, //
      pain::SpriteComponent::create(
          {.m_size = size, .layer = pain::RenderLayer::Closer}, tex), //
      pain::SpritelessComponent::createQuad(size),                    //
      pain::RotationComponent{},                                      //
      pain::Movement2dComponent{},                                    //
      pain::SAPCollider::createAABB(size, true),
      pain::ParticleSprayComponent{}, //
      Component::OrthoCamera::create(resolutionWeigh, resolutionHeight,
                                     zoomLevel), //
      pain::NativeScriptComponent{},             //
      pain::LuaScriptComponent::create(entity)   //
  );
  pain::Scene::emplaceScript<pain::OrthoCameraScript>(entity, scene);
  return entity;
}

void Player::Script::onRender(pain::Renderer2d &renderer, bool isMinimized,
                              pain::DeltaTime currentTime)
{
  auto &camCC = getComponent<Component::OrthoCamera>();
  IMGUI_PLOG([=]() { ImGui::Text("Camera zoom: %.3f", camCC.m_zoomLevel); });
}

void Player::Script::onCreate()
{
  getComponent<pain::Movement2dComponent>().m_rotationSpeed = 1.f;

  getEventDispatcher().subscribe<pain::CollisionEvent>(
      [&](const pain::CollisionEvent &e) {
        if (e.a == getEntity() || e.b == getEntity()) {
          PLOG_I("Detected collision with entity {}",
                 e.a == getEntity() ? e.b : e.a);
        }
      });
}
