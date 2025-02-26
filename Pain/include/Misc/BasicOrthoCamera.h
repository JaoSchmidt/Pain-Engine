#pragma once

#include "Core.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rotation.h"
#include "ECS/Scriptable.h"
#include "pain.h"

namespace pain
{

class OrthoCameraController
    : public ExtendedScriptableEntity<MovementComponent, TransformComponent,
                                      OrthoCameraComponent, RotationComponent>
{
public:
  void onUpdate(double deltaTimeSec);
  void onRender(double currentTime) {};
  void onEvent(const SDL_Event &e);

  inline void recalculatePosition(const glm::vec3 &position, float rotation);

private:
  float m_zoomSpeed = 0.25f;
  bool onMouseScrolled(const SDL_Event &e, OrthoCameraComponent &cc);
  bool onWindowResized(const SDL_Event &e, OrthoCameraComponent &cc);
};

} // namespace pain
