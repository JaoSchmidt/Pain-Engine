-- script = require("engine_api")
local function add(x, y)
	return x + y
end

script.onUpdate(function(self, dt)
	print("this is inside onUpdate, dt = " .. tostring(dt))
end)

script.onCreate(function()
	x, y = self:getPosition()
	print(x)
	print(y)
	print("this is onCreate function, x+y = " .. tostring(add(x, y)))
end)

script.onDestroy(function()
	print("this is onDestroy function")
end)
