#pragma once
#include "ECS/GameObject.h"

namespace pain
{
class OrthoCameraEntity : public GameObject
{
public:
  OrthoCameraEntity(Scene *scene, float aspectRatio, float zoomLevel);
};

} // namespace pain
