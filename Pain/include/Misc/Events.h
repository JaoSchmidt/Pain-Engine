// Events.h
#pragma once

#include "Core.h"
#include "ECS/EventDispatcher.h"
#include "ECS/Registry/Entity.h"
#include "glm/ext/vector_float2.hpp"
#include <sol/state.hpp>

namespace pain
{
struct ImGuiViewportChangeEvent {
  glm::vec2 newSize;
  sol::table toLuaTable(const sol::state &lua) const
  {
    return lua.create_table_with( //
        lua,                      //
        "newSize", newSize        //
    );
  }
};

struct CollisionEvent {
  reg::Entity a;
  reg::Entity b;
  glm::vec2 normal;
  float penetration;
  sol::table toLuaTable(const sol::state &lua) const
  {
    return lua.create_table_with(  //
        lua,                       //
        "a", a,                    //
        "b", b,                    //
        "normal", normal,          //
        "penetration", penetration //
    );
  }
};

#define EVENT_TYPE_LIST X(CollisionEvent, Collision)

void createScriptEventMap(sol::state &lua, reg::EventDispatcher &reg);

} // namespace pain
