#include "dummy.h"
#include "Scripting/Component.h"
#include <pain.h>

Dummy::Dummy(pain::Scene *scene, const glm::vec2 &position,
             const glm::vec2 &size, const glm::vec4 &color,
             pain::Texture *ptexture, float tilingFactor)
    : NormalEntity(*scene)
{
  createComponents(
      *scene, pain::Transform2dComponent{position},
      pain::SpriteComponent{size, color, tilingFactor, *ptexture},
      pain::LuaScriptComponent{m_entity, *scene, scene->getSharedLuaState()});
}
