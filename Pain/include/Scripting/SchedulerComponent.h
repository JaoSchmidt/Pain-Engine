#pragma once
#include "Core.h"
#include <SDL2/SDL_events.h>

#include <sol/sol.hpp>
namespace cmp
{

struct LuaScheduleTask {
  // NOTE: not sure if I have to put optional here, but it maybe a good idea
  sol::protected_function onScheduleFunction;
  float interval = 1.f;
  float elapsed = 0;
};

} // namespace cmp
