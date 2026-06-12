#pragma once
#include "ECS/Scriptable.h"

namespace pain
{

class BasicScene : public WorldObject
{
public:
  using WorldObject::WorldObject;
  void static syncSystems(Scene &scene);
};

} // namespace pain
