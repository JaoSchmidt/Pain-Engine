#include "Scripting/State.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "Scripting/Component.h"
#include "Scripting/InputManager.h"
#include <sol/object.hpp>
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
  // ------ GRAPHICS ----------------------------------------
  lua.new_usertype<glm::vec2>(
      "vec2", sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
      "x", &glm::vec2::x, "y", &glm::vec2::y);
  lua.new_usertype<glm::vec3>(
      "vec3", sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
      "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);
  lua.new_usertype<glm::vec4>(
      "vec4", sol::constructors<glm::vec4(), glm::vec4(float, float, float, float)>(),
      "r", &glm::vec4::r, "g", &glm::vec4::g, "b", &glm::vec4::b, "a", &glm::vec4::a);
  // clang-format on
  // ------ COMPONENTS ----------------------------------------
  // type returned by get_sprite(self)
  lua.new_usertype<SpriteComponent>(
      "SpriteComponent", sol::no_constructor,            //
      "m_size", &SpriteComponent::m_size,                //
      "m_color", &SpriteComponent::m_color,              //
      "m_tilingFactor", &SpriteComponent::m_tilingFactor //
      // NOTE: not going to put texture right now because too much work
  );
  // type returned by get_movement(self)
  lua.new_usertype<MovementComponent>(
      "MovementComponent", sol::no_constructor,                     //
      "m_velocity", &MovementComponent::m_velocityDir,              //
      "m_translationSpeed", &MovementComponent::m_translationSpeed, //
      "m_rotationSpeed", &MovementComponent::m_rotationSpeed);

  // type returned by get_position(self)
  lua.new_usertype<TransformComponent>(             //
      "TransformComponent", sol::no_constructor,    //
      "m_position", &TransformComponent::m_position //
  );

  lua.new_usertype<LuaScriptComponent>(
      "LuaScriptComponent", "get_position",
      [&](LuaScriptComponent &c) -> sol::object {
        if (c.hasAnyComponents<TransformComponent>())
          return sol::make_reference(
              c.getLuaState(), std::ref(c.getComponent<TransformComponent>()));
        return sol::nil;
      },
      "get_sprite",
      [&](LuaScriptComponent &c) -> sol::object {
        if (c.hasAnyComponents<SpriteComponent>())
          return sol::make_reference(
              c.getLuaState(), std::ref(c.getComponent<SpriteComponent>()));
        return sol::nil;
      },
      "get_movement",
      [&](LuaScriptComponent &c) -> sol::object {
        if (c.hasAnyComponents<MovementComponent>())
          return sol::make_reference(
              c.getLuaState(), std::ref(c.getComponent<MovementComponent>()));
        return sol::nil;
      });

  // ------ EVENTS ----------------------------------------
  // Usage in Lua: "Input.isKeyPressed(Scancode.SPACE)"
  lua.new_enum<SDL_Scancode>(
      "Scancode", //
      {
          {"SPACE", SDL_SCANCODE_SPACE},   //
          {"W", SDL_SCANCODE_W},           //
          {"A", SDL_SCANCODE_A},           //
          {"S", SDL_SCANCODE_S},           //
          {"D", SDL_SCANCODE_D},           //
          {"UP", SDL_SCANCODE_UP},         //
          {"DOWN", SDL_SCANCODE_DOWN},     //
          {"LEFT", SDL_SCANCODE_LEFT},     //
          {"RIGHT", SDL_SCANCODE_RIGHT},   //
          {"ESCAPE", SDL_SCANCODE_ESCAPE}, //
          {"RETURN", SDL_SCANCODE_RETURN}, //
          {"LSHIFT", SDL_SCANCODE_LSHIFT}, //
          {"RSHIFT", SDL_SCANCODE_RSHIFT}, //
          {"LCTRL", SDL_SCANCODE_LCTRL},   //
          {"RCTRL", SDL_SCANCODE_RCTRL},   //
                                           // Add any other scancodes you need
      });
  lua.new_usertype<InputManager>(   //
      "Input", sol::no_constructor, //
      "is_key_pressed",
      static_cast<bool (*)(SDL_Scancode)>(&InputManager::isKeyPressed));
  return lua;
};
} // namespace pain
