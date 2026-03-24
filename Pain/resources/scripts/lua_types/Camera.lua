--==============================================================
-- Camera Components
--==============================================================

---@class OrthoCameraCmp
---@field zoomLevel number           -- Zoom factor (higher = zoomed in)
---@field active boolean              -- Whether this camera is active in the render pipeline
---@field entity integer             -- Owning entity
---@field screenPosition ivec2       -- Screen-space offset (usually vec2(0,0))

---Returns the current resolution.
---@return ivec2
function OrthoCameraCmp:get_resolution() end

---Sets the resolution and updates derived values (aspect ratio, etc).
---@param width number
---@param height number
function OrthoCameraCmp:set_resolution(width, height) end

---Returns the cached view-projection matrix.
---@return mat4
function OrthoCameraCmp:get_view_projection_matrix() end

---Recomputes the view matrix from position and rotation.
---@param position vec2
---@param rotation number
function OrthoCameraCmp:recalculate_view_matrix(position, rotation) end

---Sets projection using explicit bounds.
---@overload fun(self, left:number, right:number, bottom:number, top:number)
---Sets projection using aspect ratio and zoom.
---@overload fun(self, aspectRatio:number, zoomLevel:number)
---Sets projection using resolution.
---@overload fun(self, width:number, height:number)
function OrthoCameraCmp:set_projection(...) end

---Recomputes zoom
---@param zoom number
function OrthoCameraCmp:set_zoom(zoom) end
