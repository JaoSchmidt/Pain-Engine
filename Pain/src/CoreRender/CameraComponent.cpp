/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// CameraComponent.cpp
#include "CoreRender/CameraComponent.h"
#include "CoreFiles/LogWrapper.h"
#include "glm/ext/matrix_transform.hpp"

#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pain
{
// ======================================================================== //
// Basic Camera Matrix
// ======================================================================= //

const glm::mat4 &CameraMatrices::getViewProjectionMatrix() const
{
  return m_viewProjectionCache;
}
const glm::mat4 &CameraMatrices::getProjectionMatrix() const
{
  return m_projection;
}
const glm::mat4 &CameraMatrices::getViewMatrix() const { return m_view; }

// ======================================================================== //
// Orthographic Camera Matrices
// ======================================================================= //

OrthographicMatrices::OrthographicMatrices(glm::mat4 projectionMatrix,
                                           glm::mat4 viewMatrix)
    : CameraMatrices{
          .m_projection{projectionMatrix},
          .m_view{viewMatrix},
          .m_viewProjectionCache{projectionMatrix * viewMatrix},
      } {};

} // namespace pain
void ::cmp::OrthoCamera::recalculateViewMatrix(const glm::vec2 &position,
                                               float angleRadians)
{
  // PLOG_E("Radians = {}, angle = {}", glm::radians(rotation), rotation);
  glm::mat4 transform =
      glm::translate(glm::mat4(1.0F), glm::vec3(position, 0)) *
      glm::rotate(glm::mat4(1.0F), angleRadians, glm::vec3(0, 0, 1));

  m_matrices.m_view = glm::inverse(transform);
  m_matrices.m_viewProjectionCache =
      m_matrices.m_projection * m_matrices.m_view;
}
void ::cmp::OrthoCamera::addZoom(float zoom)
{
  m_zoomLevel += zoom;
  setProjection(-m_aspectRatio * zoom, m_aspectRatio * zoom, -zoom, zoom);
}
void ::cmp::OrthoCamera::setProjection(int width, int height)
{
  setResolution(width, height);
  float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
  m_aspectRatio = aspectRatio;
  setProjection(-aspectRatio * m_zoomLevel, aspectRatio * m_zoomLevel,
                -m_zoomLevel, m_zoomLevel);
}
void ::cmp::OrthoCamera::setProjection(float aspectRatio, float zoomLevel)
{
  m_aspectRatio = aspectRatio;
  m_zoomLevel = aspectRatio;
  setProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel,
                zoomLevel);
}
void ::cmp::OrthoCamera::setProjection(float left, float right, float bottom,
                                       float top)
{
  m_matrices.m_projection = glm::ortho(left, right, bottom, top, -1.0F, 1.0F);
  m_matrices.m_viewProjectionCache =
      m_matrices.m_projection * m_matrices.m_view;
}

// ======================================================================== //
// Perspective Camera Matrices
// ======================================================================== //

pain::PerspectiveMatrices::PerspectiveMatrices(glm::mat4 perspectiveMatrix,
                                               glm::mat4 viewMatrix)
    : CameraMatrices{.m_projection{perspectiveMatrix},
                     .m_view{glm::mat4(1.0f)},
                     .m_viewProjectionCache{perspectiveMatrix * viewMatrix}} {};

void Component::PerspCamera::recalculateViewMatrix(glm::vec3 position,
                                                   glm::vec3 frontCamera)
{
  glm::vec3 frontUnit = glm::normalize(frontCamera);
  const glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
  m_matrices.m_view = glm::lookAt(position, position + frontUnit, up);
  m_matrices.m_viewProjectionCache =
      m_matrices.m_projection * m_matrices.m_view;
}

void Component::PerspCamera::setProjection(int width, int height)
{
  setResolution(width, height);
  float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
  m_aspectRatio = aspectRatio;
  setProjection(aspectRatio, m_fieldOfViewDegrees);
}

void Component::PerspCamera::setProjection(float aspectRatio,
                                           float fieldOfViewDegrees)
{
  m_matrices.m_projection = glm::perspective(glm::radians(fieldOfViewDegrees),
                                             aspectRatio, 0.01F, 100.0F);
  m_matrices.m_viewProjectionCache =
      m_matrices.m_projection * m_matrices.m_view;
}

// ======================================================================== //
// Components getters
// ======================================================================== //

const glm::ivec2 &Component::CameraResolution::getResolution() const
{
  return m_resolution;
}
void Component::CameraResolution::setResolution(int w, int h)
{
  m_resolution = glm::ivec2(w, h);
}
float cmp::CameraResolution::getAspectRatio() const { return m_aspectRatio; }
const glm::mat4 &Component::PerspCamera::getViewProjectionMatrix() const
{
  return m_matrices.m_viewProjectionCache;
}
const glm::mat4 &Component::OrthoCamera::getViewProjectionMatrix() const
{
  return m_matrices.m_viewProjectionCache;
}

// ======================================================================== //
// Components constructors
// ======================================================================== //

// ----- Orthographic ------

Component::OrthoCamera Component::OrthoCamera::create(bool active, int resWidth,
                                                      int resHeight,
                                                      float zoomLevel,
                                                      reg::Entity entity)
{
  const float aspectRatio =
      static_cast<float>(resWidth) / static_cast<float>(resHeight);

  return Component::OrthoCamera{
      active,
      zoomLevel,
      pain::OrthographicMatrices( //
          glm::ortho(-aspectRatio * zoomLevel, aspectRatio * zoomLevel,
                     -zoomLevel, zoomLevel, -1.0F, 1.0F),
          glm::mat4(1.F)),
      aspectRatio,
      resWidth,
      resHeight,
      entity};
}
Component::OrthoCamera::OrthoCamera(bool active, float zoomLevel,
                                    pain::OrthographicMatrices oc,
                                    float aspectRatio, int resWidth,
                                    int resHeight, reg::Entity entity)
    : CameraResolution{active, glm::vec2(resWidth, resHeight), aspectRatio,
                       entity},
      m_zoomLevel(zoomLevel), m_matrices(oc) {};

glm::vec2
Component::OrthoCamera::screenToWorld(int mouseX, int mouseY,
                                      const pain::Transform2dComponent &camTC)
{
  const float ndcX = (2.f * static_cast<float>(mouseX)) /
                         static_cast<float>(getResolution().x) -
                     1.f;
  const float ndcY = 1.f - (2.f * static_cast<float>(mouseY)) /
                               static_cast<float>(getResolution().y);

  const glm::vec2 localCoord =
      glm::vec2(ndcX * m_zoomLevel * m_aspectRatio, ndcY * m_zoomLevel);

  return glm::vec2(camTC.m_position.x, camTC.m_position.y) + localCoord;
}

glm::vec2
Component::OrthoCamera::screenToWorld(int mouseX, int mouseY,
                                      const pain::Transform2dComponent &camTC,
                                      const pain::RotationComponent &camRC)
{
  const float ndcX = (2.f * static_cast<float>(mouseX)) /
                         static_cast<float>(getResolution().x) -
                     1.f;
  const float ndcY = 1.f - (2.f * static_cast<float>(mouseY)) /
                               static_cast<float>(getResolution().y);

  const glm::vec2 localCoord =
      glm::vec2(ndcX * m_zoomLevel * m_aspectRatio, ndcY * m_zoomLevel);

  const float angle = camRC.m_rotationRadians;
  glm::mat2 rotation = glm::mat2(std::cos(angle), -std::sin(angle),
                                 std::sin(angle), std::cos(angle));
  return glm::vec2(camTC.m_position.x, camTC.m_position.y) +
         rotation * localCoord;
}

// ----- Perspective ------
Component::PerspCamera Component::PerspCamera::create(bool active, int resWidth,
                                                      int resHeight,
                                                      float fieldOfViewDegrees,
                                                      reg::Entity entity,
                                                      float yaw, float pitch)
{
  if (fieldOfViewDegrees > 200 || fieldOfViewDegrees < 40)
    PLOG_W("Warning. Perspective cameara FOV = {} is very {}, make sure you "
           "are not confusing values",
           fieldOfViewDegrees, fieldOfViewDegrees > 200 ? "big" : "small");
  const float aspectRatio =
      static_cast<float>(resWidth) / static_cast<float>(resHeight);
  glm::mat4 perspectiveMatrix = glm::perspective(
      glm::radians(fieldOfViewDegrees), aspectRatio, 0.01F, 100.0F);
  // calculate camera direction
  const glm::vec3 cameraFront = glm::normalize(glm::vec3( //
      cos(glm::radians(yaw)) * cos(glm::radians(pitch)),  //
      sin(glm::radians(pitch)),                           //
      sin(glm::radians(yaw)) * cos(glm::radians(pitch))   //
      ));
  const glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
  glm::mat4 viewMatrix = glm::lookAt(glm::vec3{0}, cameraFront, up);
  return Component::PerspCamera{
      active,      pain::PerspectiveMatrices(perspectiveMatrix, viewMatrix),
      aspectRatio, resWidth,
      resHeight,   fieldOfViewDegrees,
      entity};
}

Component::PerspCamera::PerspCamera(bool active, pain::PerspectiveMatrices pe,
                                    float aspectRatio, int resWidth,
                                    int resHeight, float fieldOfViewDegrees,
                                    reg::Entity entity)
    : CameraResolution{active, glm::vec2(resWidth, resHeight), aspectRatio,
                       entity},
      m_fieldOfViewDegrees(fieldOfViewDegrees), m_matrices(pe) {};
