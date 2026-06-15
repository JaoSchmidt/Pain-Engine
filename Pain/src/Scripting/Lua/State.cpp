/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// State.cpp
#include "Scripting/Lua/State.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/SpriteComponent.h"
#include "Misc/Schedule/SchedulerComponent.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"
#include "Scripting/Lua/LuaScriptComponent.h"
#include <SDL2/SDL_scancode.h>
#include <filesystem>
#include <sol/object.hpp>
#include <sol/sol.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

class InputManager
{
public:
  static inline bool isKeyPressed(SDL_Scancode scancode)
  {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    return state[scancode] != 0;
  }
};

int my_exception_handler(lua_State *L,
                         sol::optional<const std::exception &> maybe_exception,
                         sol::string_view description)
{
  // L is the lua state, which you can wrap in a state_view if necessary
  // maybe_exception will contain exception, if it exists
  // description will either be the what() of the exception or a description
  // saying that we hit the general-case catch(...)
  std::cout << "An exception occurred in a function, here's what it says ";
  if (maybe_exception) {
    std::cout << "(straight from the exception): ";
    const std::exception &ex = *maybe_exception;
    std::cout << ex.what() << std::endl;
  } else {
    std::cout << "(from the description parameter): ";
    std::cout.write(description.data(),
                    static_cast<std::streamsize>(description.size()));
    std::cout << std::endl;
  }

  // you must push 1 element onto the stack to be
  // transported through as the error object in Lua
  // note that Lua -- and 99.5% of all Lua users and libraries -- expects a
  // string so we push a single string (in our case, the description of the
  // error)
  return sol::stack::push(L, description);
}

namespace pain::luabinder
{
sol::state createLuaState()
{
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math,
                     sol::lib::string, sol::lib::io, sol::lib::coroutine,
                     sol::lib::debug, sol::lib::table);
  sol::table package = lua["package"];
#ifdef PLATFORM_IS_LINUX
  package["path"] = std::string(package["path"]) +
                    ";/usr/share/lua/5.4/?.lua"
                    ";/usr/share/lua/5.4/?/init.lua";
  package["cpath"] = std::string(package["cpath"]) + ";/usr/lib/lua/5.4/?.so";
#elif defined PLATFORM_IS_WINDOWS
  // TODO, get windows path
#endif
  lua.set_exception_handler(&my_exception_handler);

  lua.set_function("print", [&](sol::variadic_args va) {
    sol::function tostring = lua["tostring"];
    std::string out;
    for (auto arg : va) {
      sol::object result = tostring(arg);
      if (!result.valid()) {
        out += "[invalid] ";
      } else {
        out += result.as<std::string>() + " ";
      }
    }
    LUA_LOG_I("{}", out);
  });
  lua.set_function("print_error", [&](sol::variadic_args va) {
    sol::function tostring = lua["tostring"];
    std::string out;
    for (auto arg : va) {
      sol::object result = tostring(arg);
      out += result.as<std::string>() + " ";
    }
    LUA_LOG_E("{}", out);
  });
  lua.set_function("print_warning", [&](sol::variadic_args va) {
    sol::function tostring = lua["tostring"];
    std::string out;
    for (auto arg : va) {
      sol::object result = tostring(arg);
      out += result.as<std::string>() + " ";
    }
    LUA_LOG_W("{}", out);
  });
  // ------ GRAPHICS ----------------------------------------
  lua.new_usertype<glm::vec2>( //
      "vec2",
      sol::constructors<glm::vec2(), glm::vec2(float), glm::vec2(float, float),
                        glm::vec2(const glm::vec3 &)>(),
      "x", &glm::vec2::x,                                            //
      "y", &glm::vec2::y,                                            //
      "length", [](const glm::vec2 &v) { return glm::length(v); },   //
      "length2", [](const glm::vec2 &v) { return glm::length2(v); }, //
      "normalize", [](const glm::vec2 &v) { return glm::normalize(v); },
      sol::meta_function::to_string,
      [](const glm::vec2 &v) {
        return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
      },
      // OPERATORS
      sol::meta_function::addition,
      [](const glm::vec2 &a, const glm::vec2 &b) { return a + b; },
      sol::meta_function::subtraction,
      [](const glm::vec2 &a, const glm::vec2 &b) { return a - b; },
      sol::meta_function::unary_minus, //
      [](const glm::vec2 &v) { return -v; }, sol::meta_function::multiplication,
      sol::overload([](const glm::vec2 &v, float s) { return v * s; },
                    [](float s, const glm::vec2 &v) { return v * s; }),
      sol::meta_function::division,
      [](const glm::vec2 &v, float s) { return v / s; });

  lua.new_usertype<glm::vec3>(
      "vec3",
      sol::constructors<glm::vec3(), glm::vec3(float),
                        glm::vec3(float, float, float)>(),
      "x", &glm::vec3::x,                                                //
      "y", &glm::vec3::y,                                                //
      "z", &glm::vec3::z,                                                //
      "length", [](const glm::vec3 &v) { return glm::length(v); },       //
      "normalize", [](const glm::vec3 &v) { return glm::normalize(v); }, //
      "cross",
      [](const glm::vec3 &v, const glm::vec3 &w) {
        return glm::cross(v, w);
      }, //
      sol::meta_function::to_string,
      [](const glm::vec3 &v) {
        return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " +
               std::to_string(v.z) + ")";
      },

      "to_vec2", [](const glm::vec3 &v) { return glm::vec2(v.x, v.y); },
      // OPERATORS
      sol::meta_function::addition,
      [](const glm::vec3 &a, const glm::vec3 &b) { return a + b; },

      sol::meta_function::subtraction,
      [](const glm::vec3 &a, const glm::vec3 &b) { return a - b; },

      sol::meta_function::unary_minus, [](const glm::vec3 &v) { return -v; },

      sol::meta_function::multiplication,
      sol::overload([](const glm::vec3 &v, float s) { return v * s; },
                    [](float s, const glm::vec3 &v) { return v * s; }),

      sol::meta_function::division,
      [](const glm::vec3 &v, float s) { return v / s; });

  lua.new_usertype<glm::vec4>(
      "vec4",
      sol::constructors<glm::vec4(), glm::vec4(float),
                        glm::vec4(float, float, float, float)>(),
      "r", &glm::vec4::r, //
      "g", &glm::vec4::g, //
      "b", &glm::vec4::b, //
      "a", &glm::vec4::a);

  lua.new_usertype<reg::Entity>(
      "Entity", sol::constructors<reg::Entity(), reg::Entity(uint32_t)>(), //
      sol::meta_function::to_string,
      [](const reg::Entity &e) { return std::to_string(e.value); },
      sol::meta_function::equal_to,
      [](const reg::Entity &a, const reg::Entity &b) {
        return a.value == b.value;
      } //
  );

  lua.new_usertype<Color>( //
      "Color",
      sol::constructors<   //
          Color(),         //
          Color(uint32_t), //
          Color(int, int, int, int)>());

  // ------------------------------------------------------------
  //  COMPONENTS
  // ------------------------------------------------------------

  // ------ Sprite ----------------------------------------
  lua.new_usertype<pain::CircleShape>("CircleShape",
                                      sol::constructors<pain::CircleShape()>(),
                                      "radius", &pain::CircleShape::radius);
  lua.new_usertype<pain::AABBShape>("AABBShape",
                                    sol::constructors<pain::AABBShape()>(),
                                    "halfSize", &pain::AABBShape::halfSize);
  lua.new_usertype<pain::RectShape>("RectShape",
                                    sol::constructors<pain::RectShape()>(),
                                    "size", &pain::RectShape::size);
  lua.new_usertype<pain::QuadShape>("QuadShape",
                                    sol::constructors<pain::QuadShape()>(),
                                    "side", &pain::QuadShape::side);
  lua.new_usertype<pain::TriangleShape>(
      "TriangleShape", sol::constructors<pain::TriangleShape()>(), "base",
      &pain::TriangleShape::base, "height", &pain::TriangleShape::height);

#define X(name) {#name, pain::RenderLayer::name},
  lua.new_enum<pain::RenderLayer>("RenderLayer", {RENDER_LAYER_ENUM(X)});
#undef X
  lua.new_usertype<pain::SpriteCreationInfo>(
      "SpriteCreationInfo", sol::constructors<pain::SpriteCreationInfo()>(),
      "layer", &pain::SpriteCreationInfo::layer);
  lua.new_usertype<SpriteComponent>(          //
      "SpriteComponent", sol::no_constructor, //
      "layer", &SpriteComponent::layer,       //
      "create", &SpriteComponent::create      //
  );

  // ------ Movement ----------------------------------------
  // type returned by get_movement(self)
  lua.new_usertype<Movement2dComponent>(
      "Movement2dComponent", sol::no_constructor,   //
      "velocity", &Movement2dComponent::m_velocity, //
      "rotationSpeed", &Movement2dComponent::m_rotationSpeed);

  // type returned by get_position(self)
  lua.new_usertype<Transform2dComponent>(           //
      "Transform2dComponent", sol::no_constructor,  //
      "position", &Transform2dComponent::m_position //
  );

  // type returned by get_rotation(self)
  lua.new_usertype<RotationComponent>(                //
      "RotationComponent", sol::no_constructor,       //
      "angle", &RotationComponent::m_rotationRadians, //
      "rotation", &RotationComponent::m_rotation      //
  );

  // type returned by get_rotation(self)
  lua.new_usertype<::cmp::OrthoCamera>(                        //
      "OrthoCamera", sol::no_constructor,                      //
      "resolution", &::cmp::OrthoCamera::m_resolution,         //
      "active", &::cmp::OrthoCamera::m_active,                 //
      "aspectRatio", &::cmp::OrthoCamera::m_aspectRatio,       //
      "screenPosition", &::cmp::OrthoCamera::m_screenPosition, //
      "entity", &::cmp::OrthoCamera::m_entity,                 //
      "zoomLevel", &::cmp::OrthoCamera::m_zoomLevel,           //
      // methods
      "get_view_projection_matrix",
      &::cmp::OrthoCamera::getViewProjectionMatrix, //
      "recalculate_view_matrix", &::cmp::OrthoCamera::recalculateViewMatrix,

      // overloaded functions
      "set_projection",
      sol::overload(
          static_cast<void (::cmp::OrthoCamera::*)(float, float, float, float)>(
              &::cmp::OrthoCamera::setProjection),
          static_cast<void (::cmp::OrthoCamera::*)(float, float)>(
              &::cmp::OrthoCamera::setProjection),
          static_cast<void (::cmp::OrthoCamera::*)(int, int)>(
              &::cmp::OrthoCamera::setProjection)),
      // static factory
      "create", &::cmp::OrthoCamera::create);

  // ------ EVENTS ----------------------------------------
  // Usage in Lua: "Input.isKeyPressed(Scancode.SPACE)"
  lua.new_enum<SDL_Scancode>(
      "Scancode",
      {
          {"SPACE", SDL_SCANCODE_SPACE},   {"A", SDL_SCANCODE_A},
          {"B", SDL_SCANCODE_B},           {"C", SDL_SCANCODE_C},
          {"D", SDL_SCANCODE_D},           {"E", SDL_SCANCODE_E},
          {"F", SDL_SCANCODE_F},           {"G", SDL_SCANCODE_G},
          {"H", SDL_SCANCODE_H},           {"I", SDL_SCANCODE_I},
          {"J", SDL_SCANCODE_J},           {"K", SDL_SCANCODE_K},
          {"L", SDL_SCANCODE_L},           {"M", SDL_SCANCODE_M},
          {"N", SDL_SCANCODE_N},           {"O", SDL_SCANCODE_O},
          {"P", SDL_SCANCODE_P},           {"Q", SDL_SCANCODE_Q},
          {"R", SDL_SCANCODE_R},           {"S", SDL_SCANCODE_S},
          {"T", SDL_SCANCODE_T},           {"U", SDL_SCANCODE_U},
          {"V", SDL_SCANCODE_V},           {"W", SDL_SCANCODE_W},
          {"X", SDL_SCANCODE_X},           {"Y", SDL_SCANCODE_Y},
          {"Z", SDL_SCANCODE_Z},           {"NUM_0", SDL_SCANCODE_0},
          {"NUM_1", SDL_SCANCODE_1},       {"NUM_2", SDL_SCANCODE_2},
          {"NUM_3", SDL_SCANCODE_3},       {"NUM_4", SDL_SCANCODE_4},
          {"NUM_5", SDL_SCANCODE_5},       {"NUM_6", SDL_SCANCODE_6},
          {"NUM_7", SDL_SCANCODE_7},       {"NUM_8", SDL_SCANCODE_8},
          {"NUM_9", SDL_SCANCODE_9},       {"UP", SDL_SCANCODE_UP},
          {"DOWN", SDL_SCANCODE_DOWN},     {"LEFT", SDL_SCANCODE_LEFT},
          {"RIGHT", SDL_SCANCODE_RIGHT},   {"ESCAPE", SDL_SCANCODE_ESCAPE},
          {"RETURN", SDL_SCANCODE_RETURN}, {"LSHIFT", SDL_SCANCODE_LSHIFT},
          {"RSHIFT", SDL_SCANCODE_RSHIFT}, {"LCTRL", SDL_SCANCODE_LCTRL},
          {"RCTRL", SDL_SCANCODE_RCTRL},
      });
  lua.new_usertype<InputManager>(   //
      "Input", sol::no_constructor, //
      "is_key_pressed",
      static_cast<bool (*)(SDL_Scancode)>(&InputManager::isKeyPressed));

  // ------ GAME ENGINE EVENTS -----------------------------
  return lua;
};

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

} // namespace pain::luabinder
