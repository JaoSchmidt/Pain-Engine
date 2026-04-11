---@meta

-- =========================
-- ENUMS
-- =========================

---@class InterfaceMenu
InterfaceMenu = {
	SIDEBAR = 0,
	BOTTOMBAR = 1,
}

-- =========================
-- CUSTOM EDITOR
-- =========================

---@class CustomEditor
CustomEditor = {}

---Add a function to a panel
---@param name string
---@param identificator integer
---@param order integer
---@param fn fun()
function CustomEditor:add_to_panel(name, identificator, fn, order) end

---Register a panel
---@param name string
---@param split number
---@param menu InterfaceMenu
function CustomEditor:register_panel(name, split, menu) end

-- global instance injected from C++
---@type CustomEditor
CustomEditor = CustomEditor

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

-- =========================
-- IMPLOT
-- =========================

---@class ImPlot
ImPlot = {}

---@param title string
---@param width number
---@param height number
---@return boolean
function ImPlot.BeginPlot(title, width, height) end

function ImPlot.EndPlot() end

---@param x_label string
---@param y_label string
function ImPlot.SetupAxes(x_label, y_label) end

---@param label string
---@param xs number[]
---@param ys number[]
function ImPlot.PlotLine(label, xs, ys) end
