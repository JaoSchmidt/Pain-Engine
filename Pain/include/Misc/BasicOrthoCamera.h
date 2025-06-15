#pragma once

#include "Core.h"
#include "ECS/Components/Camera.h"
#include "ECS/Scriptable.h"
#include <type_traits>

namespace pain
{

class OrthoCamera
    : public NormalEntity<MovementComponent, RotationComponent,
                          TransformComponent, OrthoCameraComponent,
                          NativeScriptComponent>
{
public:
  OrthoCamera(Scene *scene, float aspectRatio, float zoomLevel);
};

class OrthoCameraController : public ExtendedEntity
{
public:
  void onUpdate(double deltaTimeSec);
  void onRender(double currentTime) {};
  void onEvent(const SDL_Event &e);
  inline void recalculatePosition(const glm::vec3 &position, float rotation);

private:
  float m_zoomSpeed = 0.25f;
  void onMouseScrolled(const SDL_Event &e, OrthoCameraComponent &cc);
  void onWindowResized(const SDL_Event &e, OrthoCameraComponent &cc);
};

} // namespace pain
