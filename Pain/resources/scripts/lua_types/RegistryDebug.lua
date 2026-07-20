---@param order? integer
function ImGuiDebugRegistry.add(name, func, order) end

---@param name string
function ImGuiDebugRegistry.remove(name) end

function ImGuiDebugRegistry.clear() end

function ImGuiDebugRegistry.renderAll() end

---@class ImGuiDebugRegistry
ImGuiDebugRegistry = {}

----------------------------------------------------------------
-- Macro-like helpers (DOCUMENTATION ONLY)
----------------------------------------------------------------

---Adds a debug callback using the current function name.
IMGUI_PLOG = function(...) end

IMGUI_PLOG_NAME = function(name, ...) end
