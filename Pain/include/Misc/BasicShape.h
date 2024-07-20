#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/GameObjWrapper.h"

namespace pain
{

class EXPORT RectangleSprite : public GameObjWrapper
{
public:
  RectangleSprite(Scene *scene, const glm::vec2 &position,
                  const glm::vec2 &size,
                  const std::shared_ptr<Texture> &ptexture, float tilingFactor,
                  const glm::vec4 &color);

  void onUpdate(double dt) {};
  void onEvent(const SDL_Event &e) {};
};

class EXPORT Rectangle : public GameObjWrapper
{
public:
  Rectangle(Scene *scene, const glm::vec2 &position, const glm::vec2 &size,
            const glm::vec4 &color);
  void onUpdate(double dt) {};
  void onEvent(const SDL_Event &e) {};
};

} // namespace pain
