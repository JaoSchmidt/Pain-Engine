#include "Scripting/StandAlone.h"
#include <lauxlib.h>
#include <lua.hpp>
#include <lualib.h>
namespace pain
{

int StandAlone(void)
{
  char buff[256];
  int error;
  lua_State *L = luaL_newstate(); /* opens Lua */
  luaL_openlibs(L); /* open libraries basic, table, I/O, string, math */

  while (fgets(buff, sizeof(buff), stdin) != NULL) {
    error = luaL_loadstring(L, buff) || lua_pcall(L, 0, 0, 0);
    if (error) {
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
      lua_pop(L, 1); /* pop error message from the stack */
    }
  }

  lua_close(L);
  return 0;
}

void error(lua_State *L, const char *fmt, ...)
{
  va_list argp;
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  lua_close(L);
  exit(EXIT_FAILURE);
}

} // namespace pain
