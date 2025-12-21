#include "Player.h"
#include "ECS/Components/Particle.h"
#include "Misc/Events.h"
#include "Physics/Collision/Collider.h"
#include "Scripting/Component.h"
#include <pain.h>

reg::Entity Player::create(pain::Scene &scene, pain::Texture &tex,
                           glm::vec2 size, int resolutionHeight,
                           int resolutionWeigh, float zoomLevel)
{
  const float aspectRatio = static_cast<float>(resolutionWeigh) /
                            static_cast<float>(resolutionHeight);
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform2dComponent{}, //
      pain::SpriteComponent{tex, size},     //
      pain::SpritelessComponent{size},      //
      pain::RotationComponent{},            //
      pain::Movement2dComponent{},          //
      pain::SAPCollider::createCollider(size, true),
      pain::ParticleSprayComponent{}, //
      pain::OrthoCameraComponent{aspectRatio, zoomLevel, resolutionWeigh,
                                 resolutionHeight}, //
      pain::NativeScriptComponent{}, pain::LuaScriptComponent{entity, scene});
  return entity;
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
