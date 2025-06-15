#include "Scripting/State.h"
#include "ECS/Components/Movement.h"
#include "ECS/Entity.h"
#include <sol/sol.hpp>

namespace pain
{
sol::state createLuaState()
{
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
  lua.set_function("print", [](sol::variadic_args va) {
    for (auto arg : va) {
      PLOG_I("{}", arg.get<std::string>());
    }
  });
  return lua;
};
} // namespace pain
