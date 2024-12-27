#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/GameObject.h"

namespace pain
{

class RectangleSprite : public GameObject
{
public:
  RectangleSprite(Scene *scene, const glm::vec2 &position,
                  const glm::vec2 &size, const glm::vec4 &color,
                  Texture *ptexture, float tilingFactor);

  void onUpdate(double dt) {};
  void onEvent(const SDL_Event &e) {};
};

class  Rectangle : public GameObject
{
public:
  Rectangle(Scene *scene, const glm::vec2 &position, const glm::vec2 &size,
            const glm::vec4 &color);
  void onUpdate(double dt) {};
  void onEvent(const SDL_Event &e) {};
};

} // namespace pain
