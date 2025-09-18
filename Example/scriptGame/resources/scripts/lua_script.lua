Script.on_update(function(self, dt)
	local tr = self:get_position()
	if tr ~= nil then
		if Input.is_key_pressed(Scancode.A) then
			tr.m_position.x = tr.m_position.x - 0.04
		end
		if Input.is_key_pressed(Scancode.D) then
			tr.m_position.x = tr.m_position.x + 0.04
		end
		if Input.is_key_pressed(Scancode.W) then
			tr.m_position.y = tr.m_position.y + 0.04
		end
		if Input.is_key_pressed(Scancode.S) then
			tr.m_position.y = tr.m_position.y - 0.04
		end
	end
end)

Script.on_create(function(self)
	-- print("this is onCreate function, x later is " .. transform.m_position.x)
end)

Script.on_event(function(self, event)
	-- print("this is on event function")
	-- print(type(event))
end)

Script.on_destroy(function(self)
	print("this is onDestroy function")
end)
