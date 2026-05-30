---@meta

-- =========================
-- ENUMS
-- =========================

---@enum InterfaceMenu
InterfaceMenu = {
	SIDEBAR = 0,
	BOTTOMBAR = 1,
}

-- =========================
-- IMGUI
-- =========================

---@class ImGui
ImGui = {}

---Draw text
---@param text string
function ImGui.Text(text) end

---Draw colored text
---@param r number
---@param g number
---@param b number
---@param a number
---@param text string
function ImGui.TextColored(r, g, b, a, text) end

function ImGui.Separator() end

---@param label string
---@return boolean
function ImGui.Button(label) end

---@param label string
---@return boolean
function ImGui.SmallButton(label) end

function ImGui.SameLine() end

function ImGui.Spacing() end

function ImGui.NewLine() end

---Returns available region size
---@return number width
---@return number height
function ImGui.GetContentRegionAvail() end

---@return number
function ImGui.GetTime() end

---@param name string
---@return boolean
function ImGui.Begin(name) end

function ImGui.End() end

---@param text string
---@param v number
---@param min number
---@param max number
---@return boolean, number
function ImGui.SliderFloat(text, v, min, max) end

---@param text string
---@param v number
---@return boolean, integer
function ImGui.InputInt(text, v) end

---@param text string
---@param x integer
---@param y integer
---@param isBorder boolean
---@return boolean
function ImGui.BeginChild(text, x, y, isBorder) end

function ImGui.EndChild() end

---@param text string
function ImGui.PushID(text) end

function ImGui.PopID() end

---@param min_x number
---@param max_x number
---@param min_y number
---@param max_y number
function ImGui.SetupAxesLimits(min_x, max_x, min_y, max_y) end

-- =========================
-- CUSTOM EDITOR
-- =========================

---@class CustomEditor
CustomEditor = {}

---Add a function to a panel
---@param name string
---@param identificator integer
---@param fn fun(imgui: ImGui, implot: ImPlot)
---@param order integer
---@overload fun(name: string, identificator: integer, fn: fun(imgui, implot))
function CustomEditor:add_to_panel(name, identificator, fn, order) end

---Register a panel
---@param name string
---@param split number
---@param menu InterfaceMenu
function CustomEditor:register_panel(name, split, menu) end

-- global instance injected from C++
---@type CustomEditor
CustomEditor = CustomEditor
