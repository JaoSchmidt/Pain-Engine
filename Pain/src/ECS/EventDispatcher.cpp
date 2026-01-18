/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "ECS/EventDispatcher.h"

// lua and native scripts should be allow to only emit/enqueue events about
// gameplay stuff
// engine should be the only one to emit/enqueue events about the
// world
namespace reg
{
using LuaListener = sol::protected_function;

void EventDispatcher::subscribe(size_t eventId, sol::function &fn)
{
  PLOG_I("Lua event {} subscription", eventId);
  m_luaSubscribers[eventId].push_back(fn);
}
void EventDispatcher::enqueue(size_t eventId, const sol::table &event)
{
  if (hasEventHandlerLua(eventId)) {
    std::vector<sol::table> &vec = m_luaPending[eventId];
    vec.emplace_back(std::move(event));
  } else {
    PLOG_W("Warning, there is no handler for custom event \"{}\", did you "
           "subcribe that event?",
           eventId);
  }
}

void EventDispatcher::trigger(size_t eventId, const sol::table &event)
{
  std::vector<sol::function> &list = m_luaSubscribers[eventId];
  for (sol::function &handler : list) {
    handler(event);
  }
}

void EventDispatcher::updateLua()
{
  for (auto &[type, pendingQueue] : m_luaPending)
    for (const sol::table &table : pendingQueue)
      trigger(type, table);
}

bool EventDispatcher::hasEventHandlerLua(size_t id) const
{
  return m_luaSubscribers.find(id) != m_luaSubscribers.end();
}
} // namespace reg
