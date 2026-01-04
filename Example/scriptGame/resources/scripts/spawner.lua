-- spawner.lua
math.randomseed(1)

local spawner_size = vec2:new(6, 2)
local spawner_position = vec2:new(6, 2)

Script.on_create(function(self)
  print("this is a on create inside spawner")
  Scheduler.every(2.0, function()
    local e = World.create_entity({
      World.Transform2d(self:get_position()),
      World.Movement2d(vec2.new(0.1, -0.2)),
      World.Rotation(),
      World.Sprite("asteroids", math.random(0, 3)),
      World.Spriteless(0.05),
      World.SAPCollider(0.1),
    })
  end)

  local sprite = self:get_sprite()
  sprite.m_size = vec2:new()
end)
