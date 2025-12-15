#include "Player.h"
#include "ECS/Components/Particle.h"
#include "Physics/Collision/Collider.h"
#include "Scripting/Component.h"
#include <pain.h>

reg::Entity Player::create(pain::Scene &scene, pain::Texture &tex,
                           int resolutionHeight, int resolutionWeigh,
                           float zoomLevel)
{
  const float aspectRatio = static_cast<float>(resolutionWeigh) /
                            static_cast<float>(resolutionHeight);
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform2dComponent{}, //
      pain::SpriteComponent{tex},           //
      pain::RotationComponent{},            //
      pain::Movement2dComponent{},          //
      pain::ParticleSprayComponent{},       //
      pain::OrthoCameraComponent{aspectRatio, zoomLevel, resolutionWeigh,
                                 resolutionHeight}, //
      pain::NativeScriptComponent{},
      pain::LuaScriptComponent{entity, scene, scene.getSharedLuaState()});
  return entity;
}
