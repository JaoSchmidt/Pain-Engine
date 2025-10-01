#include "Player.h"
#include "ECS/Components/Particle.h"
#include "Scripting/Component.h"
#include <pain.h>

Player::Player(pain::Scene *scene) : NormalEntity(*scene)
{
  createComponents(*scene, pain::TransformComponent{}, //
                   pain::SpriteComponent{},            //
                   pain::MovementComponent{},          //
                   pain::ParticleSprayComponent{},     //
                   pain::OrthoCameraComponent{},       //
                   pain::LuaScriptComponent{m_entity, getBitMask(), *scene,
                                            scene->getSharedLuaState()});
}
