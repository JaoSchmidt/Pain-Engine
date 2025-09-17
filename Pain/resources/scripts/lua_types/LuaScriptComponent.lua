---@class vec2
---@field x number
---@field y number
local vec2 = {}

---@class vec3
---@field x number
---@field y number
---@field z number
local vec3 = {}

---@class vec4
---@field r number Red component
---@field g number Green component
---@field b number Blue component
---@field a number Alpha component
local vec4 = {}

---@class SpriteComponent
---@field m_size vec2 Size of the sprite
---@field m_color vec4 Color tint of the sprite (RGBA)
---@field m_tilingFactor number Tiling factor for texture repetition
local SpriteComponent = {}

---@class MovementComponent
---@field m_velocity vec3 Velocity direction vector
---@field m_translationSpeed number Translation speed multiplier
---@field m_rotationSpeed number Rotation speed multiplier
local MovementComponent = {}

---@class TransformComponent
---@field m_position vec3 World position of the entity
local TransformComponent = {}

---@class LuaScriptComponent
local LuaScriptComponent = {}

---Get the TransformComponent of this entity
---@param self LuaScriptComponent
---@return TransformComponent|nil transform The transform component or nil if not present
function LuaScriptComponent:get_position() end

---Get the SpriteComponent of this entity
---@param self LuaScriptComponent
---@return SpriteComponent|nil sprite The sprite component or nil if not present
function LuaScriptComponent:get_sprite() end

---Get the MovementComponent of this entity
---@param self LuaScriptComponent
---@return MovementComponent|nil movement The movement component or nil if not present
function LuaScriptComponent:get_movement() end
