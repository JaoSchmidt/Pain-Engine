local function add(x, y)
	return x + y
end

print(type(script.on_update))
print(type(script))
script.on_update(function(self, dt)
	print("this is inside onUpdate, dt = " .. tostring(dt))
end)

script.on_create(function(self)
	local x, y = self:get_position()
	print(x)
	print(y)
	print("this is onCreate function, x+y = " .. tostring(add(x, y)))
end)

script.on_destroy(function(self)
	print("this is onDestroy function")
end)
