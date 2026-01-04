#include "ECS/Scene.h"
#include <sol/state.hpp>
namespace pain
{

sol::state createLuaState();
void addComponentFunctions(sol::state &solstate);
void addScheduler(sol::state &lua, Scene &worldScene);
void addEntityFunctions(sol::state &lua, Scene &worldScene);

} // namespace pain
