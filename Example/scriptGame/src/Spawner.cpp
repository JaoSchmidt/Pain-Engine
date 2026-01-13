#include "Spawner.h"
#include "Scripting/Component.h"
#include <pain.h>

reg::Entity Spawner::create(pain::Scene &scene)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(                                     //
      entity,                                                 //
      pain::Transform2dComponent{},                           //
      pain::SpriteComponent::create(),                        //
      pain::SpritelessComponent{.color = {204, 76, 51, 102}}, //
      pain::LuaScriptComponent::create(entity));
  return entity;
}
