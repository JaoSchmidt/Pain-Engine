#include "Misc/BasicShape.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"

namespace pain
{

RectangleSprite::RectangleSprite(Scene *scene, const glm::vec2 &position,
                                 const glm::vec2 &size,
                                 const std::shared_ptr<Texture> &ptexture,
                                 float tilingFactor, const glm::vec4 &color)
    : GameObject(scene)
{
  addComponent<TransformComponent>(position);
  addComponent<SpriteComponent>(size, color, tilingFactor, ptexture);
}

Rectangle::Rectangle(Scene *scene, const glm::vec2 &position,
                     const glm::vec2 &size, const glm::vec4 &color)
    : GameObject(scene)
{
  addComponent<TransformComponent>(position);
  addComponent<SpritelessComponent>(size, color);
}

} // namespace pain
