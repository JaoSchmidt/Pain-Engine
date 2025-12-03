// Asteroid.h
#pragma once

#include <glm/vec2.hpp>
#include <pain.h>

class Asteroid : public pain::NormalEntity<pain::Transform2dComponent, //
                                           pain::Movement2dComponent,  //
                                           pain::RotationComponent,    //
                                           pain::SpriteComponent,      //
                                           pain::SpritelessComponent,  //
                                           pain::ColliderComponent>    //
{
public:
  Asteroid(pain::Scene &scene, pain::TextureSheet &texSheet, short id,
           glm::vec2 pos, glm::vec2 vel, glm::vec2 size);
  Asteroid(pain::Scene &scene, pain::TextureSheet &texSheet, short id,
           glm::vec2 pos, glm::vec2 vel, const float radius);
};
