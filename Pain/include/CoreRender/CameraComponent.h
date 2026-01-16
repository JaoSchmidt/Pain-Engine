/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once
#include "pch.h"

#include "Core.h"
#include "ECS/Components/ComponentManager.h"

#include <glm/gtc/matrix_transform.hpp>
namespace pain
{

// ================================================================= //
// Basic Camera Matrices required to change the view/perspective
// ================================================================= //

class CameraMatrices
{
public:
  const glm::mat4 &getProjectionMatrix() const;
  const glm::mat4 &getViewMatrix() const;
  const glm::mat4 &getViewProjectionMatrix() const;

  glm::mat4 m_projection;
  glm::mat4 m_view = glm::mat4(1.f);
  glm::mat4 m_viewProjectionCache;
};

class OrthographicMatrices : public CameraMatrices
{
public:
  OrthographicMatrices(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
};

class PerspectiveMatrices : public CameraMatrices
{
public:
  PerspectiveMatrices(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix);
};

} // namespace pain
// ================================================================= //
// OrthoCamera Component for the ECS
// ================================================================= //

namespace cmp
{
struct CameraResolution {
  glm::ivec2 m_resolution;
  float m_aspectRatio;
  const glm::ivec2 &getResolution() const;
  void setResolution(int w, int h);
};

struct OrthoCamera : CameraResolution {
  using tag = pain::tag::OrthoCamera;
  float m_zoomLevel = 1.0f;
  pain::OrthographicMatrices m_matrices;

  const glm::mat4 &getViewProjectionMatrix() const;
  static OrthoCamera create(int resWidth, int resHeight, float zoomLevel);
  void recalculateViewMatrix(const glm::vec2 &m_position,
                             const float m_rotation);

  // reset the entire projection using screen edges
  void setProjection(float left, float right, float bottom, float top);
  // slightly easier projection setter that uses only aspectRatio and zoomLevel
  void setProjection(float aspectRatio, float zoomLevel);
  // even easier projection setter that recalculates aspectRatio and
  // uses the internal zoomLevel
  void setProjection(int width, int height);

  OrthoCamera() = delete;

private:
  OrthoCamera(float zoomLevel, pain::OrthographicMatrices oc, float aspectRatio,
              int resWidth, int resHeight);
};

struct PerspCamera : CameraResolution {
  pain::PerspectiveMatrices m_matrices;

  const glm::mat4 &getViewProjectionMatrix() const;
  static PerspCamera create(int resWidth, int resHeight,
                            float fieldOfViewDegrees);
  void recalculateViewMatrix(glm::vec3 m_position, glm::vec3 m_rotation);
  // reset the entire projection using aspectRatio and fieldOfViewDegrees
  void setProjection(float aspectRatio, float fieldOfViewDegrees);
  PerspCamera() = delete;

private:
  PerspCamera(pain::PerspectiveMatrices pe, float aspectRatio, int resWidth,
              int resHeight);
};

} // namespace cmp
namespace Component = cmp;
