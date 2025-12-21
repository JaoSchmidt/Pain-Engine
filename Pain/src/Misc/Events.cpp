// Events.cpp
#include "Misc/Events.h"
#include "ECS/EventDispatcher.h"
namespace pain
{
enum class EventType : size_t {
#define X(eventclass, eventname) eventname,
  EVENT_TYPE_LIST
#undef X
      Count
};

sol::state &createScriptEventMap(sol::state &lua, reg::EventDispatcher &ed)
{
  lua["Event"] = lua.create_table();
  lua["Event"]["subscribe"] = [&](EventType eventType, sol::function fn) {
    switch (eventType) {
#define X(eventclass, eventname)                                               \
  case EventType::eventname:                                                   \
    ed.subscribe<eventclass>(fn);                                              \
    break;
      EVENT_TYPE_LIST
#undef X
    case EventType::Count:
    default:
      PLOG_W(
          "Warning, I wasn't able to subscribe Event type \"{}\", perhaps "
          "you try to subscribe something custom? E.g. Event.subscribeCustom()",
          static_cast<int>(eventType));
      break;
    }
  };
  lua["Event"]["subscribeCustom"] = [&](const size_t customEventId,
                                        sol::function fn) {
    ed.subscribe(customEventId, fn);
  };

  lua["Event"]["enqueueCustom"] = [&](const size_t customEventId,
                                      const sol::table event) {
    ed.enqueue(customEventId, event);
  };

  lua.new_enum<EventType>("EventType",
                          {
#define X(eventclass, eventname) {#eventname, EventType::eventname}, //
                              EVENT_TYPE_LIST                        //
#undef X                                                             //
                          });
  return lua;
}
#undef EVENT_TYPE_LIST
} // namespace pain
