---@class DeltaTime
---@field raw number Raw time value in nanoseconds
DeltaTime = {}

---Create a new DeltaTime
---@overload fun(): DeltaTime
---@overload fun(nanoseconds: number): DeltaTime
---@return DeltaTime
function DeltaTime.new(nanoseconds) end

---Create a DeltaTime from seconds
---@param seconds number
---@return DeltaTime
function DeltaTime.create_seconds(seconds) end

---Get time in seconds (double precision)
---@return number
function DeltaTime:seconds() end

---Get time in milliseconds (double precision)
---@return number
function DeltaTime:milliseconds() end

---Get time in microseconds (double precision)
---@return number
function DeltaTime:microseconds() end

---Get time in nanoseconds (double precision)
---@return number
function DeltaTime:nanoseconds() end

---Add two DeltaTime values
---@param other DeltaTime
---@return DeltaTime
function DeltaTime:__add(other) end

---Subtract two DeltaTime values
---@param other DeltaTime
---@return DeltaTime
function DeltaTime:__sub(other) end

---Multiply DeltaTime by a scalar
---@overload fun(self: DeltaTime, scalar: number): DeltaTime
---@overload fun(scalar: number, self: DeltaTime): DeltaTime
---@return DeltaTime
function DeltaTime:__mul(scalar) end

---Divide DeltaTime by a scalar
---@param scalar number
---@return DeltaTime
function DeltaTime:__div(scalar) end

---Less-than comparison
---@param other DeltaTime
---@return boolean
function DeltaTime:__lt(other) end

---Less-than or equal comparison
---@param other DeltaTime
---@return boolean
function DeltaTime:__le(other) end

---Equality comparison
---@param other DeltaTime
---@return boolean
function DeltaTime:__eq(other) end
