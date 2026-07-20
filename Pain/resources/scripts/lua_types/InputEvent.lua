---@class InputEvent
---@field type integer # Raw SDL event type (e.g. SDL_KEYDOWN, SDL_MOUSEMOTION, etc.)
---@field key fun(self: InputEvent): integer # Returns the SDL scancode of the key involved in the event
---@field is_key_down fun(self: InputEvent): boolean # True if this event is a key press
---@field is_key_up fun(self: InputEvent): boolean # True if this event is a key release
---@field mouse_x fun(self: InputEvent): integer # Mouse X position (valid for mouse events)
---@field mouse_y fun(self: InputEvent): integer # Mouse Y position (valid for mouse events)
---@field is_whell_rolling fun(self: InputEvent): boolean # True if this is a mouse wheel event
---@field whell_y fun(self: InputEvent): boolean # True if this is a mouse wheel event
---@field is_quit fun(self: InputEvent): boolean # True if this is a quit event (window close, etc.)

---Input event object provided by the engine.

---This is a lightweight wrapper around the internal SDL event system.
---Not all fields are valid for every event type, so you should always
---check the event type or helper functions before accessing data.
------------------------------------------------------------------

---Example:
---`lua ---local e = Input.poll_event() ---if e and e:is_key_down() then ---    print("Key pressed:", e:key()) ---end ---`
InputEvent = {}
