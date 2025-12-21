--- @enum EventType
EventType = {
  Collision = 1,
}

---@class CollisionEvent
---@field a number
---@field b number
---@field normal vec2
---@field penetration number

--- @class Events
Event = {}

---@overload fun(event: EventType, callback: fun(collision_event: CollisionEvent))
---@param event EventType
---@param callback fun(e: table)
function Event.subscribe(event, callback) end
