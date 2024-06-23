#include "Misc/BasicShape.h"
#include "ECS/Components/Sprite.h"
#include "ECS/GameObjWrapper.h"

namespace pain
{

BasicSpriteEntity::BasicSpriteEntity(Scene *scene, glm::vec2 &position,
                                     const glm::vec2 &size,
                                     const std::shared_ptr<Texture> &texture,
                                     float tilingFactor, glm::vec4 tintColor)
    : GameObjWrapper(scene)
{
  addComponent<TransformComponent>();
  addComponent<SpriteRendererComponent>();
}

void BasicSpriteEntity::render() {}

} // namespace pain
