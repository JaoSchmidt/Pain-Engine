#include "Spawner.h"
#include "Scripting/Component.h"
#include <pain.h>

reg::Entity Spawner::create(pain::Scene &scene)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(              //
      entity,                          //
      pain::Transform2dComponent{},    //
      pain::SpriteComponent::create(), //
      pain::SpritelessComponent{.m_color =
                                    glm::vec4(0.8f, 0.3f, 0.2f, 0.4f)}, //
      pain::LuaScriptComponent::create(entity, scene));
  return entity;
}
