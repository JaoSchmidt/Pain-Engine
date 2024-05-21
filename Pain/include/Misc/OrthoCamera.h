#pragma once
#include "pch.h"

#include "Core.h"
#include "CoreFiles/DeltaTime.h"
#include "CoreRender/Camera.h"
#include "ECS/GameObjWrapper.h"

namespace pain
{

class EXPORT OrthographicCameraEntity : public GameObjWrapper
{
public:
  OrthographicCameraEntity(Scene &scene, float aspectRatio);

  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &e);

  // OrthographicCamera &getCamera() { return *m_camera; }
  const OrthographicCamera &getCamera() const { return m_camera; }

  inline void recalculatePosition(const glm::vec3 &position, float rotation);

private:
  bool onMouseScrolled(const SDL_Event &e);
  bool onWindowResized(const SDL_Event &e);

  float m_aspectRatio = 800.0 / 600.0;
  float m_zoomSpeed = 0.25f;
  float m_zoomLevel = 1.0f;
  OrthographicCamera m_camera;
};

} // namespace pain
