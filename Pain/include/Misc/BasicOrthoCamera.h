// BasicOrthoCamera.h
#pragma once

#include "Core.h"
#include "ECS/Components/Camera.h"
#include "ECS/Scriptable.h"
#include <type_traits>

namespace pain
{

class Scene;
class OrthoCamera : public NormalEntity
{
public:
  MOVABLE(OrthoCamera)
  NONCOPYABLE(OrthoCamera)
  OrthoCamera(Scene *scene, int resolutionHeight, int resolutionWeight,
              float zoomLevel);
};

class OrthoCameraScript : public ExtendedEntity
{
public:
  using ExtendedEntity::ExtendedEntity;
  void onUpdate(double deltaTimeSec);
  void onCreate();
  void onEvent(const SDL_Event &e);
  inline void recalculatePosition(const glm::vec2 &position, float rotation);

private:
  float m_zoomSpeed = 0.25f;
  void onMouseScrolled(const SDL_Event &e, OrthoCameraComponent &cc);
  void onWindowResized(const SDL_Event &e, OrthoCameraComponent &cc);
};

} // namespace pain
