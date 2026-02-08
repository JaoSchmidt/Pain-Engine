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
                                               const float rotation)
{
  glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), glm::vec3(position, 0)) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1));

  m_matrices.m_view = glm::inverse(transform);
  m_matrices.m_viewProjectionCache =
      m_matrices.m_projection * m_matrices.m_view;
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
  m_matrices.m_projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
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
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  // m_viewMatrix = glm::inverse(transform);
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
                                             aspectRatio, 0.1f, 100.0f);
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

Component::OrthoCamera Component::OrthoCamera::create(int resWidth,
                                                      int resHeight,
                                                      float zoomLevel,
                                                      reg::Entity entity)
{
  const float aspectRatio =
      static_cast<float>(resWidth) / static_cast<float>(resHeight);

  return Component::OrthoCamera{
      zoomLevel,
      pain::OrthographicMatrices( //
          glm::ortho(-aspectRatio * zoomLevel, aspectRatio * zoomLevel,
                     -zoomLevel, zoomLevel, -1.0f, 1.0f),
          glm::mat4(1.f)),
      aspectRatio,
      resWidth,
      resHeight,
      entity};
}
Component::OrthoCamera::OrthoCamera(float zoomLevel,
                                    pain::OrthographicMatrices oc,
                                    float aspectRatio, int resWidth,
                                    int resHeight, reg::Entity entity)
    : CameraResolution{glm::vec2(resWidth, resHeight), aspectRatio, entity},
      m_zoomLevel(zoomLevel), m_matrices(oc) {};

// ----- Perspective ------

Component::PerspCamera Component::PerspCamera::create(int resWidth,
                                                      int resHeight,
                                                      float fieldOfViewDegrees,
                                                      reg::Entity entity)
{
  const float aspectRatio =
      static_cast<float>(resWidth) / static_cast<float>(resHeight);
  glm::mat4 perspectiveMatrix = glm::perspective(
      glm::radians(fieldOfViewDegrees), aspectRatio, 0.1f, 10.0f);
  glm::mat4 viewMatrix = glm::mat4(1.f);
  return Component::PerspCamera{
      pain::PerspectiveMatrices(perspectiveMatrix, viewMatrix), aspectRatio,
      resWidth, resHeight, entity};
}

Component::PerspCamera::PerspCamera(pain::PerspectiveMatrices pe,
                                    float aspectRatio, int resWidth,
                                    int resHeight, reg::Entity entity)
    : CameraResolution{glm::vec2(resWidth, resHeight), aspectRatio, entity},
      m_matrices(pe) {};
