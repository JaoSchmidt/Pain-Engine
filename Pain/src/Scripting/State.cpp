// State.cpp
#include "Scripting/State.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/Sprite.h"
#include "Physics/MovementComponent.h"
#include "Scripting/Component.h"
#include "Scripting/InputManager.h"
#include "Scripting/SchedulerComponent.h"
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
      LUA_LOG_I("{}", arg.get<std::string>());
    }
  });
  lua.set_function("print_error", [](sol::variadic_args va) {
    for (auto arg : va) {
      LUA_LOG_E("{}", arg.get<std::string>());
    }
  });
  lua.set_function("print_warning", [](sol::variadic_args va) {
    for (auto arg : va) {
      LUA_LOG_W("{}", arg.get<std::string>());
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
  lua.new_usertype<Movement2dComponent>(
      "Movement2dComponent", sol::no_constructor,     //
      "m_velocity", &Movement2dComponent::m_velocity, //
      "m_rotationSpeed", &Movement2dComponent::m_rotationSpeed);

  // type returned by get_position(self)
  lua.new_usertype<Transform2dComponent>(             //
      "Transform2dComponent", sol::no_constructor,    //
      "m_position", &Transform2dComponent::m_position //
  );

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

  // ------ GAME ENGINE EVENTS -----------------------------
  return lua;
};

void addComponentFunctions(sol::state &lua)
{
  lua.new_usertype<pain::LuaScriptComponent>(
      "LuaScriptComponent", "get_position",
      [&](pain::LuaScriptComponent &c) -> sol::object {
        if (c.hasAnyComponents<pain::Transform2dComponent>())
          return sol::make_reference(
              lua, std::ref(c.getComponent<pain::Transform2dComponent>()));
        return sol::nil;
      },
      "get_sprite",
      [&](LuaScriptComponent &c) -> sol::object {
        if (c.hasAnyComponents<SpriteComponent>())
          return sol::make_reference(
              lua, std::ref(c.getComponent<SpriteComponent>()));
        return sol::nil;
      },
      "get_movement",
      [&](LuaScriptComponent &c) -> sol::object {
        if (c.hasAnyComponents<Movement2dComponent>())
          return sol::make_reference(
              lua, std::ref(c.getComponent<Movement2dComponent>()));
        return sol::nil;
      });
}
// NOTE: maybe I should move this into a single space?
void addScheduler(sol::state &lua, Scene &worldScene)
{
  sol::table scheduler_api = lua.create_table();
  scheduler_api["every"] = [&](float interval, sol::function f) {
    reg::Entity e = worldScene.createEntity();
    worldScene.createComponents(
        e, cmp::LuaScheduleTask{.onScheduleFunction = std::move(f),
                                .interval = interval});
  };
  lua["Scheduler"] = scheduler_api;
}

} // namespace pain
