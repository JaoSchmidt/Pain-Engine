#include "Player.h"
#include "ECS/Components/Particle.h"
#include "Physics/Collision/Collider.h"
#include "Scripting/CollisionCallback.h"
#include "Scripting/Component.h"
#include <pain.h>

Player::Player(pain::Scene &scene, pain::Texture &tex) : NormalEntity(scene)
{
  createComponents(
      scene, pain::TransformComponent{},  //
      pain::SpriteComponent{tex},         //
      pain::MovementComponent{},          //
      pain::ParticleSprayComponent{},     //
      pain::OrthoCameraComponent{},       //
      pain::CollisionCallbackComponent{}, //
      pain::LuaScriptComponent{m_entity, scene, scene.getSharedLuaState()});
}
