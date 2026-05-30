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

---@class ImPlotCondEnum
---@field None 0
---@field Always 1
---@field Once 2
---@type ImPlotCondEnum
ImPlotCond = { None = 0, Always = 1, Once = 2 }

---@param x_label string
---@param y_label string
function ImPlot.SetupAxes(x_label, y_label) end

---@param x_min number
---@param x_max number
---@param y_min number
---@param y_max number
---@param plot_cond? ImPlotCondEnum
function ImPlot.SetNextAxisLimits(x_min, x_max, y_min, y_max, plot_cond) end

---@param x_min number
---@param x_max number
---@param y_min number
---@param y_max number
---@param plot_cond? ImPlotCondEnum
function ImPlot.SetupAxesLimits(x_min, x_max, y_min, y_max, plot_cond) end

---@param label string
---@param xs number[]
---@param ys number[]
function ImPlot.PlotScatter(label, xs, ys) end

---@param label string
---@param xs number[]
---@param ys number[]
---@param color? Color
function ImPlot.PlotLine(label, xs, ys, color) end

---@class VecDouble
local VecDouble = {}

---Create a new vector
---@overload fun(): VecDouble
---@overload fun(size: integer): VecDouble
---@overload fun(size: integer, value: number): VecDouble
---@return VecDouble
function VecDouble.new(...) end

---Returns the number of elements
---@return integer
function VecDouble:size() end

---Returns true if empty
---@return boolean
function VecDouble:empty() end

---Clears the vector
function VecDouble:clear() end

---Appends a value to the end
---@param value number
function VecDouble:push_back(value) end

---Get element at index (0-based)
---@param index integer
---@return number
function VecDouble:__index(index) end

---Set element at index (0-based)
---@param index integer
---@param value number
function VecDouble:__newindex(index, value) end

---Length operator (#vec)
---@return integer
function VecDouble:__len() end

---Downsample vector data into a smaller vector
---@param finalSize integer
---@return VecDouble
function VecDouble:downsample(finalSize) end

---@return number
function VecDouble:min() end

---@return number
function VecDouble:max() end

----------------------------------------------------------------

---@class VecFloat
local VecFloat = {}

---@overload fun(): VecFloat
---@overload fun(size: integer): VecFloat
---@overload fun(size: integer, value: number): VecFloat
---@return VecFloat
function VecFloat.new(...) end

---@return integer
function VecFloat:size() end

---@return boolean
function VecFloat:empty() end

function VecFloat:clear() end

---@param value number
function VecFloat:push_back(value) end

---@param index integer
---@return number
function VecFloat:__index(index) end

---@param index integer
---@param value number
function VecFloat:__newindex(index, value) end

---@return integer
function VecFloat:__len() end

---@return number
function VecFloat:min() end

---@return number
function VecFloat:max() end

----------------------------------------------------------------

---@class VecInt
local VecInt = {}

---@overload fun(): VecInt
---@overload fun(size: integer): VecInt
---@overload fun(size: integer, value: integer): VecInt
---@return VecInt
function VecInt.new(...) end

---@return integer
function VecInt:size() end

---@return boolean
function VecInt:empty() end

function VecInt:clear() end

---@param value integer
function VecInt:push_back(value) end

---@param index integer
---@return integer
function VecInt:__index(index) end

---@param index integer
---@param value integer
function VecInt:__newindex(index, value) end

---@return integer
function VecInt:__len() end

---@return number
function VecInt:min() end

---@return number
function VecInt:max() end
