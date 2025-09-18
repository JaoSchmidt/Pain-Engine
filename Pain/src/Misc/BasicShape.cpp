#include "Misc/BasicShape.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"

namespace pain
{

RectangleSprite::RectangleSprite(Scene *scene, const glm::vec2 &position,
                                 const glm::vec2 &size, const glm::vec4 &color,
                                 Texture *ptexture, float tilingFactor)
    : NormalEntity(*scene)
{
  createComponents(*scene, TransformComponent{position},
                   SpriteComponent{size, color, tilingFactor, *ptexture});
}

Rectangle::Rectangle(Scene *scene, const glm::vec2 &position,
                     const glm::vec2 &size, const glm::vec4 &color)
    : NormalEntity(*scene)
{
  createComponents(*scene, TransformComponent{position},
                   SpritelessComponent{size, color});
}

} // namespace pain
