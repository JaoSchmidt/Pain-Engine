function f(x, y)
	return (x ^ 2 * math.sin(y)) / (1 - x)
end
function onCreate()
	print("this is onCreate function")
end
function onUpdate(deltaTime)
	print("this is onUpdate function with a deltaTime of " .. deltaTime)
end
function onDestroy()
	print("this is onDestroy function")
end
-- print("this isn't inside any function")
-- print("A")
-- print("B")
-- print("C")
-- print("D")
-- print("E")
-- print("F")
-- print("G")
