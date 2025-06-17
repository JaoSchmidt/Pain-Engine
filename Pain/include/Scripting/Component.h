#pragma once
#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/Scriptable.h"
#include <SDL2/SDL_events.h>

#include <sol/sol.hpp>

namespace pain
{
struct LuaScriptComponent : public ExtendedEntity {
  const char *m_scriptPath;
  LuaScriptComponent(const char *scriptPath, sol::state &solState);
  void InitializeScript();
  void onCreate();
  void onUpdate(double deltaTime);
  void onDestroy();
  void onEvent(const SDL_Event *event);

  NONCOPYABLE(LuaScriptComponent)
  LuaScriptComponent &operator=(LuaScriptComponent &&other) noexcept;
  LuaScriptComponent(LuaScriptComponent &&other) = delete;
  ~LuaScriptComponent() = default;

private:
  sol::state &m_lua;
  sol::environment m_env;

  sol::protected_function m_onUpdate;
  sol::protected_function m_onCreate;
  sol::protected_function m_onDestroy;

  // ===================================================================
  // private functions/methods

  // expose getComponent<T> to the lua state
  template <typename T> void setTemplateFunction(const char *name)
  {
    P_ASSERT(hasAnyComponents<T>(),
             "Component is not registered, did you forget to add to the "
             "ComponentManager?");
    // NOTE: Unfortunetly, I can't directly set the actual function because the
    // return can't be used to differentiate:
    // https://github.com/ThePhD/sol2/issues/98#issuecomment-950206267
    m_env.set_function(
        name, [](LuaScriptComponent &c) -> T & { return c.getComponent<T>(); });
  }
};

} // namespace pain
