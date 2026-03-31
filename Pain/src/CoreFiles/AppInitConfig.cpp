#include "CoreFiles/AppInitConfig.h"

namespace pain::luabinder
{

void bindAppInitConfig(sol::state &lua, const AppInit &init)
{
  lua.new_usertype<AppInit>(
      "AppInitConfig", "title", sol::readonly(&AppInit::title), //
      "defaultWidth", sol::readonly(&AppInit::defaultWidth),    //
      "defaultHeight", sol::readonly(&AppInit::defaultHeight),  //
      "defaultZoom2d", sol::readonly(&AppInit::defaultZoom2d)   //
  );
  lua["AppInitConfig"] = &init;
}

} // namespace pain::luabinder
