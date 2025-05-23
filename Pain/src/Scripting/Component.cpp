#include "Scripting/Component.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
}

namespace pain
{
int custom_print(lua_State *L)
{
  int nargs = lua_gettop(L);
  for (int i = 1; i <= nargs; i++) {
    const char *str = lua_tostring(L, i);
    if (str) {
      // TODO: custom spdlog::logger just for lua environment
      PLOG_I("{}", str);
    }
  }
  return 0;
}

LuaScriptComponent::LuaScriptComponent(const char *scriptPath)
    : m_scriptPath(scriptPath)
{
  m_L = luaL_newstate();
  luaL_openlibs(m_L);
  lua_pushcfunction(m_L, custom_print);
  lua_setglobal(m_L, "print");
  if (luaL_dofile(m_L, m_scriptPath) != LUA_OK) {
    PLOG_E("Error loading Lua script: {}", lua_tostring(m_L, -1));
    lua_pop(m_L, 1);
  } else {
    PLOG_I("Loading Lua script: {}", lua_tostring(m_L, -1));
  }
}
void LuaScriptComponent::onCreate() { callIfExists("onCreate"); }
void LuaScriptComponent::onUpdate(double deltaTime)
{
  lua_getglobal(m_L, "onUpdate");
  if (lua_isfunction(m_L, -1)) {
    lua_pushnumber(m_L, deltaTime);
    if (lua_pcall(m_L, 0, 0, 0) != LUA_OK) {
      PLOG_E("Lua error (onUpdate): %s", lua_tostring(m_L, -1));
      lua_pop(m_L, 1);
    }
  } else {
    PLOG_W("Function onUpdate wasn't find inside lua script {}", m_scriptPath);
    lua_pop(m_L, 1);
  }
}
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
      PLOG_E("Lua error (%s): %s", name, lua_tostring(m_L, -1));
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
      PLOG_E("Lua error (%s): %s", lua_tostring(m_L, -1));
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
      PLOG_I("{}, ", lua_tostring(L, i));
      break;
    }
    case LUA_TBOOLEAN: { /* Booleans */
      PLOG_I("{}, ", lua_toboolean(L, i) ? "true" : "false");
      break;
    }
    case LUA_TNUMBER: { /* numbers */
      PLOG_I("{}, ", lua_tonumber(L, i));
      break;
    }
    default: { /* other values */
      PLOG_I("{}, ", lua_typename(L, t));
      break;
    }
    }
  }
}

} // namespace pain
