#include "Wall.h"
#include <pain.h>

namespace Wall
{

reg::Entity create(pain::Scene &scene, const glm::vec2 &pos,
                   const glm::vec2 &size)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(
      entity, pain::Transform2dComponent{pos},                            //
      pain::SpritelessComponent{size, glm::vec4(0.6f, 0.6f, 0.6f, 0.4f)}, //
      pain::SAPCollider::createCollider(entity, size));
  return entity;
}
} // namespace Wall
