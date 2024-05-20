#pragma once
#include "pch.h"

#include "Core.h"
#include "CoreFiles/DeltaTime.h"
#include "CoreRender/Camera.h"

namespace pain
{

class EXPORT OrthographicCameraController
{
public:
  OrthographicCameraController(float aspectRatio);

  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &e);

  // OrthographicCamera &getCamera() { return *m_camera; }
  const OrthographicCamera &getCamera() const { return m_camera; }

  inline void setPosition(const glm::vec3 &position);
  inline const glm::vec3 &getPosition() const { return m_position; }

  inline void setRotation(float rotation);
  inline float getRotation() const { return m_rotation; }

private:
  bool onMouseScrolled(const SDL_Event &e);
  bool onWindowResized(const SDL_Event &e);

  float m_aspectRatio = 800.0 / 600.0;
  float m_zoomLevel = 1.0f;
  OrthographicCamera m_camera;

  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  float m_rotation = 0.0f;
  float m_translationSpeed = 1.0f;
  float m_rotationSpeed = 180.0f;
  float m_zoomSpeed = 0.25f;
};

} // namespace pain
