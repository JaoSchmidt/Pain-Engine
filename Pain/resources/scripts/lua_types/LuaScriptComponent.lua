---@class vec2
---@field x number
---@field y number
---@operator add(vec2): vec2
---@operator sub(vec2): vec2
---@operator mul(number): vec2
---@operator div(number): vec2
---@operator unm: vec2
vec2 = {}

---@param self vec2
---@return number
function vec2.length(self) end

---@param self vec2
---@return number
function vec2.length2(self) end

---@param self vec2
---@return vec2
function vec2.normalize(self) end

-- =======================================================

---@class vec3
---@field x number
---@field y number
---@field z number
---@operator add(vec3): vec3
---@operator sub(vec3): vec3
---@operator mul(number): vec3
---@operator div(number): vec3
---@operator unm: vec3
vec3 = {}

---Retrieve vec.xy from vec3
---@overload fun(): vec3
---@return vec2
function vec3:to_vec2() end

---Create a new vec3
---@overload fun(): vec3
---@overload fun(x: number, y: number, z: number): vec3
---@return vec3
function vec3.new(x, y, z) end

---Returns the length (magnitude) of the vector.
---@param self vec3
---@return number
function vec3.length(self) end

---Returns a normalized (unit length) vector.
---@param self vec3
---@return vec3
function vec3.normalize(self) end

---Computes the cross product between two vectors.
---@param self vec3
---@param other vec3
---@return vec3
function vec3.cross(self, other) end

-- =======================================================

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
---@field size vec2 Size of the sprite
---@field color vec4 Color tint of the sprite (RGBA)
---@field tilingFactor number Tiling factor for texture repetition
SpriteComponent = {}

---@class Movement2dComponent
---@field velocity vec2 Velocity direction vector
---@field translationSpeed number Translation speed multiplier
---@field rotationSpeed number Rotation speed multiplier
Movement2dComponent = {}

---@class Movement3dComponent
---@field velocity vec3 Velocity direction vector
---@field translationSpeed number Translation speed multiplier
---@field rotationSpeed number Rotation speed multiplier
Movement3dComponent = {}

---@class Transform2dComponent
---@field position vec2 World position of the entity
Transform2dComponent = {}

---@class Transform3dComponent
---@field position vec3 World position of the entity
Transform3dComponent = {}

---@class RotationComponent
---@field rotation number World rotation of the entity
RotationComponent = {}

---@class LuaScriptComponent
LuaScriptComponent = {}
