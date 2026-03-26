---@class ScriptSelf
---@field entity Entity
ScriptSelf = {}

---@class Script
---@field on_create fun(callback: fun(self: ScriptSelf))
---@field on_update fun(callback: fun(self: ScriptSelf, dt: number))
---@field on_destroy fun(callback: fun(self: ScriptSelf))
---@field on_event fun(callback: fun(self: ScriptSelf, event: InputEvent))
Script = {}
