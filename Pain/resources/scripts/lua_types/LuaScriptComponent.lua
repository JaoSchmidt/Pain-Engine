---@class vec2
---@field x number
---@field y number
vec2 = {}

---Create a new vec2
---@overload fun(): vec2
---@overload fun(x: number, y: number): vec2
---@return vec2
function vec2.new(x, y) end

---@class vec3
---@field x number
---@field y number
---@field z number
vec3 = {}

---Create a new vec3
---@overload fun(): vec3
---@overload fun(x: number, y: number, z: number): vec3
---@return vec3
function vec3.new(x, y, z) end

---@class vec4
---@field r number Red component
---@field g number Green component
---@field b number Blue component
---@field a number Alpha component
vec4 = {}

---Create a new vec4
---@overload fun(): vec4
---@overload fun(r: number, g: number, b: number, a: number): vec4
---@return vec4
function vec4.new(r, g, b, a) end

---@class SpriteComponent
---@field m_size vec2 Size of the sprite
---@field m_color vec4 Color tint of the sprite (RGBA)
---@field m_tilingFactor number Tiling factor for texture repetition
SpriteComponent = {}

---@class MovementComponent
---@field m_velocity vec3 Velocity direction vector
---@field m_translationSpeed number Translation speed multiplier
---@field m_rotationSpeed number Rotation speed multiplier
MovementComponent = {}

---@class TransformComponent
---@field m_position vec3 World position of the entity
TransformComponent = {}

---@class LuaScriptComponent
LuaScriptComponent = {}

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
