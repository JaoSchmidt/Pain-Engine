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

---@overload fun(eventId: integer, callback: fun())
---@param eventId integer
---@param callback fun(eventId, e: table)
function Event.subscribeCustom(eventId, callback) end

---@overload fun(eventId: integer, callback: fun())
---@param eventId integer
---@param t table
function Event.enqueueCustom(eventId, t) end
