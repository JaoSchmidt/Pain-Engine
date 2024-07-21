#pragma once

#include "Core.h"
#include "CoreRender/Camera.h"
#include "ECS/GameObject.h"

namespace pain
{

class EXPORT OrthoCameraController : public ScriptableEntity
{
public:
  OrthoCameraController() : ScriptableEntity(){};

  void onCreate();
  void onDestroy();
  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &e);

  // OrthographicCamera &getCamera() { return *m_camera; }

  inline void recalculatePosition(const glm::vec3 &position, float rotation);

private:
  bool onMouseScrolled(const SDL_Event &e);
  bool onWindowResized(const SDL_Event &e);
};

class EXPORT OrthoCameraEntity : public GameObject
{
public:
  OrthoCameraEntity(Scene *scene, float aspectRatio, float zoomLevel);
  const OrthographicCamera &getCamera() const { return m_camera; }
  OrthographicCamera &getCamera() { return m_camera; }

  float m_aspectRatio = 800.0 / 600.0;
  float m_zoomSpeed = 0.25f;
  float m_zoomLevel = 1.0f;

private:
  OrthographicCamera m_camera;
};
} // namespace pain
