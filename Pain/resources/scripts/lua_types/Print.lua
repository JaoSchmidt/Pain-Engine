---
---Receives any number of arguments and prints their values to `stdout`, converting each argument to a string following the same rules of [tostring](http://www.lua.org/manual/5.1/manual.html#pdf-tostring).
---The function print is not intended for formatted output, but only as a quick way to show a value, for instance for debugging. For complete control over the output, use [string.format](http://www.lua.org/manual/5.1/manual.html#pdf-string.format) and [io.write](http://www.lua.org/manual/5.1/manual.html#pdf-io.write).
---
---
---
---@param ... any
function print_error(...) end

---
---Receives any number of arguments and prints their values to `stdout`, converting each argument to a string following the same rules of [tostring](http://www.lua.org/manual/5.1/manual.html#pdf-tostring).
---The function print is not intended for formatted output, but only as a quick way to show a value, for instance for debugging. For complete control over the output, use [string.format](http://www.lua.org/manual/5.1/manual.html#pdf-string.format) and [io.write](http://www.lua.org/manual/5.1/manual.html#pdf-io.write).
---
---
---
---@param ... any
function print_warning(...) end
