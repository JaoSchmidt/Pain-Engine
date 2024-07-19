#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/GameObjWrapper.h"

namespace pain
{

class BasicSpriteEntity : public GameObjWrapper
{
public:
  BasicSpriteEntity(Scene *scene) : GameObjWrapper(scene){};
};

class EXPORT RectangleSprite : BasicSpriteEntity
{
public:
  RectangleSprite(Scene *scene, const glm::vec2 &position,
                  const glm::vec2 &size,
                  const std::shared_ptr<Texture> &ptexture, float tilingFactor,
                  const glm::vec4 &color);

  void onUpdate(double dt) {};
  void onEvent(const SDL_Event &e) {};
};

} // namespace pain
