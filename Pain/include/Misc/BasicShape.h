#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/GameObject.h"

namespace pain
{

class RectangleSprite : public GameObject<TransformComponent, SpriteComponent>
{
public:
  RectangleSprite(Scene *scene, const glm::vec2 &position,
                  const glm::vec2 &size, const glm::vec4 &color,
                  Texture *ptexture, float tilingFactor);
};

class Rectangle : public GameObject<TransformComponent, SpriteComponent>
{
public:
  Rectangle(Scene *scene, const glm::vec2 &position, const glm::vec2 &size,
            const glm::vec4 &color);
};

} // namespace pain
