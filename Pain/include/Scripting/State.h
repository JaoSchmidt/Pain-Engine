#include "ECS/WorldScene.h"
#include <sol/state.hpp>
namespace pain
{

sol::state createLuaState();
template <typename AbstractScene>
void addComponentFunctions(sol::state &solstate, AbstractScene &worldScene);
void addScheduler(sol::state &lua, Scene &worldScene);
void addEntityFunctions(sol::state &lua, Scene &worldScene);
} // namespace pain
