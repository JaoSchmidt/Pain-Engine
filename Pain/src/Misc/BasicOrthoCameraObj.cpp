#include "Misc/BasicOrthoCameraObj.h"

#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rotation.h"

namespace pain
{

OrthoCameraEntity::OrthoCameraEntity(Scene *scene, float aspectRatio,
                                     float zoomLevel)
    : GameObject(scene)
{
  addComponents(MovementComponent{}, RotationComponent{}, TransformComponent{},
                OrthoCameraComponent{aspectRatio, zoomLevel});
};
} // namespace pain
