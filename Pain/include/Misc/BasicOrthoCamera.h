// BasicOrthoCamera.h
#pragma once

#include "Core.h"
#include "CoreRender/CameraComponent.h"
#include "ECS/Scriptable.h"
#include <type_traits>

namespace pain
{

namespace Dummy2dCamera
{
reg::Entity create(pain::Scene &scene, int resolutionHeight,
                   int resolutionWeigh, float zoomLevel);

reg::Entity createBasicCamera(pain::Scene &scene, int resolutionHeight,
                              int resolutionWeigh, float zoomLevel);
} // namespace Dummy2dCamera

class OrthoCameraScript : public ExtendedEntity
{
public:
  using ExtendedEntity::ExtendedEntity;
  void onUpdate(DeltaTime deltaTimeSec);
  void onCreate();
  void onEvent(const SDL_Event &e);
  inline void recalculatePosition(const glm::vec2 &position, float rotation);

protected:
  float m_zoomSpeed = 0.25f;
  void onMouseScrolled(const SDL_Event &e, Component::OrthoCamera &cc);
  void onWindowResized(const SDL_Event &e, Component::OrthoCamera &cc);
};

} // namespace pain
