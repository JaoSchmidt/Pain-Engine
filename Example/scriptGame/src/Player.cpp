/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Player.h"
#include "GUI/ImGuiDebugRegistry.h"

reg::Entity Player::create(pain::Scene &scene, pain::Texture &tex,
                           glm::vec2 initialPos, glm::vec2 size,
                           int resolutionHeight, int resolutionWeigh,
                           float zoomLevel)
{
  reg::Entity entity = scene.createEntity("ScriptGamePlayer");
  scene.createComponents(                             //
      entity, pain::Transform2dComponent{initialPos}, //
      pain::SpriteComponent::create(
          {.m_size = size, .layer = pain::RenderLayer::E}, tex), //
      pain::SpritelessComponent::createQuad(size),               //
      pain::RotationComponent{},                                 //
      pain::Movement2dComponent{},                               //
      pain::SAPCollider::createAABB(size, true),
      pain::ParticleSprayComponent::create({
          .randAngleFactor = 20.f,
          .capacity = 100,
      }),
      OrthoCameraComponent::create(resolutionWeigh, resolutionHeight,
                                     zoomLevel, entity), //
      pain::NativeScriptComponent{},                     //
      pain::LuaScriptComponent::create(entity)           //
  );
  pain::Scene::emplaceScript<pain::OrthoCameraScript>(entity, scene);
  return entity;
}

void Player::Script::onRender(pain::RenderApi &renderer, 
                              pain::DeltaTime currentTime)
{
  auto &camCC = getComponent<OrthoCameraComponent>();
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
