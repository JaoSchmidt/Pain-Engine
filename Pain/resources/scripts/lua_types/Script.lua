---@class Script
---@field on_create fun(callback: fun(self: LuaScriptComponent))
---@field on_update fun(callback: fun(self: LuaScriptComponent, dt: number))
---@field on_destroy fun(callback: fun(self: LuaScriptComponent))
---@field on_event fun(callback: fun(self: LuaScriptComponent, event: SDL_Event))
Script = {}
