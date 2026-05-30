#pragma once
#include "CoreFiles/LogWrapper.h"
#include "SDL_events.h"
#include "sol/sol.hpp"

namespace pain
{
namespace luabinder
{

struct LuaInputEvent {
  const SDL_Event *e;

  inline uint32_t type() const { return e->type; }

  // Keyboard
  inline SDL_Scancode key() const { return e->key.keysym.scancode; }
  inline bool is_key_down() const { return e->type == SDL_KEYDOWN; }
  inline bool is_key_up() const { return e->type == SDL_KEYUP; }
  inline bool is_whell_rolling() const { return e->type == SDL_MOUSEWHEEL; }

  // Mouse
  inline int mouse_x() const { return e->motion.x; }
  inline int mouse_y() const { return e->motion.y; }

  inline int whell_y() const { return e->wheel.y; }
  // Quit
  inline bool is_quit() const { return e->type == SDL_QUIT; }

  static void bindInputEvents(sol::state &lua);
};

} // namespace luabinder
} // namespace pain
