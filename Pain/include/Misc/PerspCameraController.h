#pragma once
#include "pch.gch"

#include "Core.h"
#include "CoreFiles/DeltaTime.h"
#include "CoreRender/Camera.h"

namespace pain
{

class EXPORT PerspectiveCameraController
{
public:
  PerspectiveCameraController(float windowWidth, float windowHeight,
                              float fieldOfViewDegrees);

  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &e);

  const PerspectiveCamera &getCamera() const { return m_camera; }

  inline void setPosition(const glm::vec3 &position)
  {
    m_position = position;
    m_camera.RecalculateViewMatrix(m_position, m_rotation);
  }
  inline const glm::vec3 &getPosition() const { return m_position; }

  inline void setRotation(glm::vec3 rotation)
  {
    m_rotation = rotation;
    m_camera.RecalculateViewMatrix(m_position, m_rotation);
  }
  inline glm::vec3 getRotation() const { return m_rotation; }

private:
  bool onMouseScrolled(const SDL_Event &e);
  bool onWindowResized(const SDL_Event &e);

  float m_aspectRatio = 800.0 / 600.0;
  float m_zoomLevel = 1.0f;
  PerspectiveCamera m_camera;

  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  glm::vec3 m_rotation = {0.0f, 0.0f, 0.0f};
  float m_fieldOfViewDegrees = 60.0f;
  float m_translationSpeed = 1.0f;
  float m_rotationSpeed = 180.0f;
  float m_zoomSpeed = 0.25f;
};

} // namespace pain
