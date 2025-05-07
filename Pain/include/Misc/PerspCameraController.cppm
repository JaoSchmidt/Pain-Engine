module;
#include "CoreFiles/LogWrapper.h"
#include <SDL_events.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
export module pain.PerspCameraController;
import pain.DeltaTime;
import pain.Camera;

export namespace pain
{

class PerspectiveCameraController
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
    m_camera.RecalculateViewMatrix(m_position, m_cameraFront);
  }
  inline const glm::vec3 &getPosition() const { return m_position; }

  inline void setFrontVector(glm::vec3 rotation)
  {
    m_cameraFront = rotation;
    m_camera.RecalculateViewMatrix(m_position, m_cameraFront);
  }
  inline glm::vec3 getFrontVector() const { return m_cameraFront; }

private:
  // events
  void onMouseMoved(const SDL_Event &e);
  void onMouseButtonUp(const SDL_Event &e);
  void onMouseScrolled(const SDL_Event &e);
  void onWindowResized(const SDL_Event &e);
  // void setMovementState(bool isMoving);
  template <bool IsMoving> void setMovementState();

  // window
  float m_aspectRatio = 800.0 / 600.0;
  float m_windowWidth = 800.0;
  float m_windowHeight = 600.0;
  PerspectiveCamera m_camera;
  bool m_isMovementEnable = true;

  // vectors
  glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
  glm::vec3 m_cameraFront = {0.0f, 0.0f, 0.0f};
  glm::vec3 m_cameraUp = {0.0f, 1.0f, 0.0f};

  // degrees
  float m_fieldOfViewDegrees = 60.0f;
  float m_yaw = 90.0f;
  float m_pitch = 0.0f;

  // derivates
  float m_translationSpeed = 1.0f;
  float m_sensitivitySpeed = 0.01f;
  float m_zoomSpeed = 10.0f;
};

} // namespace pain
