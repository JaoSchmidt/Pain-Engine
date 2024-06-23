#pragma once
#include "pch.h"

#include "Core.h"
#include "ECS/GameObjWrapper.h"

namespace pain
{

class EXPORT BasicSpriteEntity : public GameObjWrapper
{
public:
  BasicSpriteEntity(Scene *scene);

  inline void render();
};

} // namespace pain
