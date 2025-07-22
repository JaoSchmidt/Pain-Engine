#include "Scripting/State.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "Scripting/Component.h"
#include <sol/sol.hpp>

namespace pain
{
sol::state createLuaState()
{
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
  lua.set_function("print", [](sol::variadic_args va) {
    for (auto arg : va) {
      PLOG_I("{}", arg.get<std::string>());
    }
  });
  // clang-format off
  lua.new_usertype<glm::vec2>(
      "vec2", sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
      "x", &glm::vec2::x, "y", &glm::vec2::y);
  lua.new_usertype<glm::vec3>(
      "vec3", sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
      "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);
  lua.new_usertype<glm::vec4>(
      "vec4", sol::constructors<glm::vec4(), glm::vec4(float, float, float, float)>(),
      "r", &glm::vec4::r, "g", &glm::vec4::g, "b", &glm::vec4::b, "a", &glm::vec4::a);


  lua.new_usertype<SpriteComponent>(
        "SpriteComponent", sol::constructors<SpriteComponent()>(), //
        "m_size", &SpriteComponent::m_size,                        //
        "m_color", &SpriteComponent::m_color,                      //
        "m_tilingFactor", &SpriteComponent::m_tilingFactor         //
        // NOTE: not going to put texture right now because too much work
    );
  lua.new_usertype<MovementComponent>(
        "MovementComponent", sol::constructors<MovementComponent()>(), //
        "m_velocity", &MovementComponent::m_velocityDir,               //
        "m_translationSpeed", &MovementComponent::m_translationSpeed,  //
        "m_rotationSpeed", &MovementComponent::m_rotationSpeed);

  lua.new_usertype<TransformComponent>(
        "TransformComponent", sol::constructors<TransformComponent()>(),
        "m_position", &TransformComponent::m_position);

  lua.new_usertype<LuaScriptComponent>("LuaScriptComponent");
  // clang-format on
  return lua;
};
} // namespace pain
