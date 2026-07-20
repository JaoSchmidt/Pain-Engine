/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Basic3dPlayer.h
#pragma once
#include "Core.h"
#include "CoreRender/CameraComponent.h"
#include "ECS/Scriptable.h"

namespace pain
{
namespace Dummy3dCamera
{
reg::Entity create(pain::Scene &scene, int resolutionWidth,
                   int resolutionHeight, float fieldOfViewDegrees, float yaw,
                   float pitch);

reg::Entity createBasicCamera(pain::Scene &scene, int resolutionWidth,
                              int resolutionHeight, float fieldOfViewDegrees,
                              float yaw, float pitch);
} // namespace Dummy3dCamera

class PerspCameraScript : public WorldObject
{
public:
  PerspCameraScript(reg::Entity entity, Scene &scene, float yaw, float pitch);
  using WorldObject::WorldObject;
  // PerspCameraScript(reg::Entity entity, Scene &scene);
  void onUpdate(DeltaTime deltaTimeSec);
  void onCreate();
  void onEvent(const SDL_Event &e);

  inline glm::vec3 getFrontVector() const { return m_cameraFront; }

protected:
  // events
  void onMouseMoved(const SDL_Event &e);
  void onMouseButtonUp(const SDL_Event &e);
  void onMouseScrolled(const SDL_Event &e);
  // void setMovementState(bool isMoving);
  template <bool IsMoving> void setMovementState();

  // window
  bool m_isMovementEnable = true;

  // vectors
  glm::vec3 m_cameraFront = {0.0f, 0.0f, 0.0f};
  glm::vec3 m_cameraUp = {0.0f, -1.0f, 0.0f};

  // degrees
  float m_yaw = -1.0f;
  float m_pitch = -1.0f;

  // derivates
  float m_sensitivitySpeed = 0.01f;
  float m_zoomSpeed = 10.0f;
  float m_moveSpeed = 100.f;
};

} // namespace pain
