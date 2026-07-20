---@meta

--==============================================================
-- Render Layer
--==============================================================

---@enum RenderLayer
--- Represent layer draw order, bigger numbers = closer
RenderLayer = {
	Distant = 0,
	FurtherBack = 1,
	Background = 2,
	Default = 3,
	Closer = 4,
	MuchCloser = 5,
	TouchingCamera = 6,
}

---@class LuaComponentDesc
---@field bit integer
---@field emplace fun(entity: integer, bitmask: integer)
---@field onEmplace? fun(entity: integer)

---@class World
World = {}

--==============================================================
-- Entity Creation
--==============================================================

---Creates an entity with the given components.
---
---All components are first collected to build the archetype,
---then applied to the entity.
---
---@param components table<number, LuaComponentDesc>
---@return Entity entity
function World.create_entity(components) end

--==============================================================
-- Sprite Component
--==============================================================

---@class SpriteShapeQuad
---@field side? number

---@class SpriteShapeRect
---@field size vec2

---@class SpriteShapeTriangle
---@field base number
---@field height number

---@alias SpriteShape
---| { type: "quad", side?: number }
---| { type: "rect", size: vec2 }
---| { type: "triangle", base: number, height: number }

---@class SpriteCreationInfo
---@field layer? RenderLayer          -- Rendering order
---@field shape? SpriteShape          -- Shape descriptor (defaults to quad)

---Creates a Sprite component.
---
---This is a descriptor-based API. All configuration is passed through a table.
---
---If no shape is provided, a default quad is used.
---
---Lua usage:
---```lua
----- default sprite (quad)
---World.Sprite()
---
----- only layer
---World.Sprite({
---    layer = RenderLayer.Closer
---})
---
----- quad
---World.Sprite({
---    shape = { type = "quad", side = 0.5 }
---})
---
----- rect
---World.Sprite({
---    shape = { type = "rect", size = vec2.new(1, 2) }
---})
---
----- triangle
---World.Sprite({
---    shape = { type = "triangle", base = 1, height = 1 }
---})
---
----- full example
---World.Sprite({
---    layer = RenderLayer.Background,
---    shape = { type = "rect", size = vec2.new(2, 1) }
---})
---```
---
---@param info? SpriteCreationInfo
---@return LuaComponentDesc
function World.Sprite(info) end

--==============================================================
-- Movement2d Component
--==============================================================

---Creates a Movement2d component.
---
---Lua usage:
---```lua
---World.Movement2d()
---World.Movement2d(vec2.new(1,0))
---World.Movement2d(vec2.new(1,0), 2.0)
---```
---
---@param velocity? vec2
---@param rotationSpeed? number
---@return LuaComponentDesc
function World.Movement2d(velocity, rotationSpeed) end

--==============================================================
-- Rotation Component
--==============================================================

---Creates a Rotation component.
---
---Lua usage:
---```lua
---World.Rotation()
---World.Rotation(3.14)
---```
---
---@param angle? number
---@param rotation? vec3
---@return LuaComponentDesc
function World.Rotation(angle, rotation) end

---
---@param angle? number
---@param rotation? vec3
---@return LuaComponentDesc
function World.Rotation(angle, rotation) end

---
---@return LuaComponentDesc
function World.Rotation() end

--==============================================================
-- Transform2d Component
--==============================================================

---Creates a Transform2d component.
---
---Lua usage:
---```lua
---World.Transform2d()
---World.Transform2d(vec2.new(1,2))
---```
---
---@param position? vec2
---@return LuaComponentDesc
function World.Transform2d(position) end

--==============================================================
-- SAPCollider Component
--==============================================================

---Creates an AABB collider.
---
---Lua usage:
---```lua
---World.SAPCollider(vec2.new(1,1))
---World.SAPCollider(vec2.new(1,1), true)
---World.SAPCollider(vec2.new(1,1), true, vec2.new(0.5, 0))
---```
---@param size vec2
---@param isTrigger? boolean
---@param offset? vec2
---@return LuaComponentDesc
function World.SAPCollider(size, isTrigger, offset) end

---Creates a Circle collider.
---
---Lua usage:
---```lua
---World.SAPCollider(0.5)
---World.SAPCollider(0.5, true)
---World.SAPCollider(0.5, true, vec2.new(0,1))
---```
---
---@param radius number
---@param isTrigger? boolean
---@param offset? vec2
---@return LuaComponentDesc
function World.SAPCollider(radius, isTrigger, offset) end

--==============================================================
-- Material Component
--==============================================================

---@class Material
---Opaque handle to a material resource created via Engine.Material.
---Provides access to runtime material properties (color, texture, etc).

---Creates a Material component.
---
---Lua usage:
---```lua
----- default material
---World.Material()
---
----- from name (must exist in Engine.Material)
---World.Material("myMaterial")
---
----- from existing material object
---local mat = Engine.Material.get("myMaterial")
---World.Material(mat)
---```
---
---@overload fun(): LuaComponentDesc
---@overload fun(name: string): LuaComponentDesc
---@overload fun(material: Material): LuaComponentDesc
---@param arg? string|Material
---@return LuaComponentDesc
function World.Material(arg) end

--==============================================================
-- LuaScript Component
--==============================================================

---@class LuaScriptComponent
---Internal component that binds a Lua script to an entity.
---
---The script is expected to return a table with optional lifecycle callbacks:
---- `onCreate(self)`
---- `onUpdate(self, dt)`
---- `onDestroy(self)`
---
---These functions are automatically detected and invoked by the engine.

---@class LuaScriptInstance
---@field entity integer
---@field onCreate? fun(self: LuaScriptInstance)
---@field onUpdate? fun(self: LuaScriptInstance, dt: number)
---@field onDestroy? fun(self: LuaScriptInstance)

---Creates a LuaScript component.
---
---This only attaches the component to the entity.
---To actually bind a script file, use `emplaceLuaScript`.
---
---Lua usage:
---```lua
----- attach empty LuaScript component
---World.LuaScript()
---```
---
---@return LuaComponentDesc
function World.LuaScript() end

--==============================================================
-- Lua Script Binding
--==============================================================

---Binds a Lua script to an entity that already has a LuaScript component.
---
---This will:
---1. Load the script file
---2. Bind it to the entity
---3. Call `onCreate(self)` if defined
---
---The script must return a table representing the script instance.
---
---Lua usage:
---```lua
---local e = World.create_entity({
---    World.LuaScript()
---})
---
--- World.emplaceLuaScript(e, "scripts/player.lua")
---```
---
---@param entity Entity        -- Target entity
---@param scriptPath string     -- Path to Lua script file
---@param init_args table     -- Arguments to pass to the script
---@overload fun (entity:Entity, scriptPath:string)
function World.emplace_lua_script(entity, scriptPath, init_args) end

--==============================================================
-- OrthoCamera Component Creation
--==============================================================

---@class OrthoCameraCreationInfo
---@field active? boolean     -- Defaults to true
---@field width? number       -- See config.ini
---@field height? number      -- See config.ini
---@field zoom? number        -- See internalConfig.ini

---Creates an OrthoCamera component.
---
---Lua usage:
---```lua
----- default camera
---World.OrthoCamera()
---
----- positional arguments
---World.OrthoCamera(true, 800, 600, 5.0)
---
----- table-style
---World.OrthoCamera({
---    active = true,
---    width = 1280,
---    height = 720,
---    zoom = 1.5
---})
---```
---
---@overload fun(): LuaComponentDesc
---@overload fun(active: boolean, width: number, height: number, zoom: number): LuaComponentDesc
---@param info? OrthoCameraCreationInfo
---@return LuaComponentDesc
function World.OrthoCamera(info) end

---@class Entity
---@field value integer # Engine entity identifier (opaque handle)

---Gets the 2D transform (position) component of an entity.
---Returns `nil` if the entity does not have this component.
---@param e Entity
---@return Transform2dComponent|nil
---@overload fun(self: ScriptSelf): Transform2dComponent|nil
function World.get_2d_position(e) end

---Gets the sprite component of an entity.
---Returns `nil` if the entity does not have this component.
---@param e Entity
---@return SpriteComponent|nil
---@overload fun(self: ScriptSelf): SpriteComponent|nil
function World.get_sprite(e) end

---Gets the 2D movement component of an entity.
---Returns `nil` if the entity does not have this component.
---@param e Entity
---@return Movement2dComponent|nil
---@overload fun(self: ScriptSelf): Movement2dComponent|nil
function World.get_2d_movement(e) end

---Gets the rotation component of an entity.
---Returns `nil` if the entity does not have this component.
---@param e Entity
---@return RotationComponent|nil
---@overload fun(self: ScriptSelf): RotationComponent|nil
function World.get_rotation(e) end

---Gets the Lua script component attached to an entity.
---Returns `nil` if the entity does not have this component.
---@param e Entity
---@return ScriptSelf|nil
---@overload fun(self: ScriptSelf): ScriptSelf|nil
function World.get_lua_script(e) end

---Gets the orthographic camera component of an entity.
---Returns `nil` if the entity does not have this component.
---@param e Entity
---@return OrthoCameraCmp|nil
---@overload fun(self: ScriptSelf): OrthoCameraCmp|nil
function World.get_ortho_camera(e) end
