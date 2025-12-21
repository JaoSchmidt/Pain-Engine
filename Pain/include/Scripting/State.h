#include <sol/state.hpp>
namespace pain
{

sol::state createLuaState();
void addComponentFunctions(sol::state &solstate);

} // namespace pain
