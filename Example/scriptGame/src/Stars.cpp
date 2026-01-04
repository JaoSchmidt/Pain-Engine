#include "Stars.h"
#include <pain.h>

namespace Stars
{
reg::Entity create(pain::Scene &scene, pain::TextureSheet &texSheet,
                   unsigned short id, glm::vec2 &pos)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity,
                         pain::Transform2dComponent{glm::vec3(pos, 0.f)}, //
                         pain::SpriteComponent::create(texSheet, id));
  return entity;
}
reg::Entity create(pain::Scene &scene, pain::Texture &tex, unsigned short id,
                   glm::vec2 &pos)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity,
                         pain::Transform2dComponent{glm::vec3(pos, 0.f)}, //
                         pain::SpriteComponent::create(tex));
  return entity;
}
} // namespace Stars
