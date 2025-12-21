// Events.cpp
#include "Misc/Events.h"
#include "ECS/EventDispatcher.h"
namespace pain
{

sol::state &createScriptEventMap(sol::state &lua, reg::EventDispatcher &ed)
{
  lua["Event"] = lua.create_table();
#define X(eventclass, eventname)                                               \
  lua["Event"]["subscribe" #eventname] = [&](sol::function fn) {               \
    ed.subscribe<eventclass>(fn);                                              \
  };
  EVENT_TYPE_LIST
#undef X

  // lua["Event"]["subscribeCollision"] = [&](sol ::function fn) {
  //   ed.subscribe<CollisionEvent>(fn);
  // };

  return lua;
}
#undef EVENT_TYPE_LIST
} // namespace pain
