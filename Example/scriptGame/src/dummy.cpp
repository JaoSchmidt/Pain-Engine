#include "dummy.h"
#include <pain.h>

Dummy::Dummy(pain::Scene *scene, const glm::vec2 &position,
             const glm::vec2 &size, const glm::vec4 &color,
             pain::Texture *ptexture, float tilingFactor)
    : GameObject(scene)
{
  createComponents(
      pain::TransformComponent{position},
      pain::SpriteComponent{size, color, tilingFactor, ptexture},
      pain::LuaScriptComponent{"resources/scripts/lua_script.lua"});
}
