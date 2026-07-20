#include "Events/LuaInputEvent.h"

namespace pain::luabinder
{
void LuaInputEvent::bindInputEvents(sol::state &lua)
{
  lua.new_usertype<LuaInputEvent>(                          //
      "InputEvent",                                         //
      "type", &LuaInputEvent::type,                         //
      "key", &LuaInputEvent::key,                           //
      "is_key_down", &LuaInputEvent::is_key_down,           //
      "is_key_up", &LuaInputEvent::is_key_up,               //
      "is_whell_rolling", &LuaInputEvent::is_whell_rolling, //
      "mouse_x", &LuaInputEvent::mouse_x,                   //
      "mouse_y", &LuaInputEvent::mouse_y,                   //
      "whell_y", &LuaInputEvent::whell_y,                   //
      "is_quit", &LuaInputEvent::is_quit                    //
  );
}
} // namespace pain::luabinder
