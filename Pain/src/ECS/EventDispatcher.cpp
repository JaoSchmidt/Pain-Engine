#include "ECS/EventDispatcher.h"

// lua and native scripts should be allow to only emit/enqueue events about
// gameplay stuff
// engine should be the only one to emit/enqueue events about the
// world
namespace reg
{
using LuaListener = sol::protected_function;

void EventDispatcher::subscribe(size_t hash, sol::function &fn)
{
  PLOG_I("Lua event {} subscription", hash);
  m_luaSubscribers[hash].push_back(fn);
}
void EventDispatcher::enqueue(size_t hash, sol::table &event)
{
  if (hasEventHandlerLua(hash)) {
    std::vector<sol::table> &vec = m_luaPending[hash];
    vec.emplace_back(std::move(event));
  } else {
    PLOG_W("Warning, there is no handler for custom event \"{}\", did you "
           "subcribe that event?",
           hash);
  }
}

void EventDispatcher::trigger(size_t hash, sol::table &event)
{
  std::vector<sol::function> &list = m_luaSubscribers[hash];
  for (sol::function &handler : list) {
    handler(event);
  }
}

void EventDispatcher::updateLua()
{
  for (auto &[type, pendingQueue] : m_luaPending)
    for (sol::table &table : pendingQueue)
      trigger(type, table);
}

bool EventDispatcher::hasEventHandlerLua(size_t id) const
{
  return m_luaSubscribers.find(id) != m_luaSubscribers.end();
}
} // namespace reg
