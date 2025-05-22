#include "Scripting/StandAlone.h"
#include "CoreFiles/LogWrapper.h"

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

namespace pain
{

LuaScriptComponent::LuaScriptComponent(const char *scriptPath)
    : m_scriptPath(scriptPath)
{
  m_L = luaL_newstate();
  if (luaL_dofile(m_L, m_scriptPath) != LUA_OK) {
    fprintf(stderr, "Error loading Lua script: %s\n", lua_tostring(m_L, -1));
    lua_pop(m_L, 1);
  }
}
void LuaScriptComponent::onCreate() {}
void LuaScriptComponent::onUpdate(double deltaTime) {}
void LuaScriptComponent::onDestroy() { callIfExists("onDestroy"); }
void LuaScriptComponent::onEvent(const SDL_Event *event) {}
LuaScriptComponent::~LuaScriptComponent()
{
  if (m_L) {
    lua_close(m_L);
    m_L = nullptr;
  }
}

void LuaScriptComponent::callIfExists(const char *name)
{
  lua_getglobal(m_L, name);
  if (lua_isfunction(m_L, -1)) {
    if (lua_pcall(m_L, 0, 0, 0) != LUA_OK) {
      fprintf(stderr, "Lua error (%s): %s\n", name, lua_tostring(m_L, -1));
      lua_pop(m_L, 1);
    }
  } else {
    PLOG_W("Function {} wasn't find inside lua script {}", name, m_scriptPath);
    lua_pop(m_L, 1);
  }
}

int LuaScriptComponent::standAlone()
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
      return 1;
    }
  }

  lua_close(L);
  return 0;
}

void LuaScriptComponent::stackDump(lua_State *L)
{
  int i;
  int top = lua_gettop(L);     /* depth of the stack */
  for (i = 1; i <= top; i++) { /* repeat for each level */
    int t = lua_type(L, i);
    switch (t) {
    case LUA_TSTRING: { /* strings */
      printf("'%s'", lua_tostring(L, i));
      break;
    }
    case LUA_TBOOLEAN: { /* Booleans */
      printf(lua_toboolean(L, i) ? "true" : "false");
      break;
    }
    case LUA_TNUMBER: { /* numbers */
      printf("%g", lua_tonumber(L, i));
      break;
    }
    default: { /* other values */
      printf("%s", lua_typename(L, t));
      break;
    }
    }
    printf(" "); /* put a separator */
  }
  printf("\n"); /* end the listing */
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
