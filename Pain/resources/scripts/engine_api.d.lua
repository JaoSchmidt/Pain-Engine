---@class LuaEntity
---@field getPosition fun(self: LuaEntity): Vector2

---@class Vector2
---@field x number
---@field y number

---@class ScriptAPI
---@field on_update fun(callback: fun(self: LuaEntity, dt: number))
---@field on_create fun(callback: fun(self: LuaEntity))
---@field on_destroy fun(callback: fun(self: LuaEntity))

---@type ScriptAPI
script = {}
