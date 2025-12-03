#include "Stars.h"
#include <pain.h>

Stars::Stars(pain::Scene &scene, pain::TextureSheet &texSheet, short id,
             glm::vec2 &pos)
    : NormalEntity(scene)
{
  createComponents(scene, pain::Transform2dComponent{glm::vec3(pos, 0.f)}, //
                   pain::SpriteComponent{texSheet, id});
}
Stars::Stars(pain::Scene &scene, pain::Texture &tex, short id, glm::vec2 &pos)
    : NormalEntity(scene)
{
  createComponents(scene, pain::Transform2dComponent{glm::vec3(pos, 0.f)}, //
                   pain::SpriteComponent{tex});
}
