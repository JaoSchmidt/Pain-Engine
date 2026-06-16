/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** CameraComponent.h */
#pragma once

#include "Core.h"
#include "ECS/Components/ComponentManager.h"
#include "pch.h"

#include <glm/gtc/matrix_transform.hpp>

namespace pain
{

/**
 * ============================================================================
 * **Camera Matrices**
 * ============================================================================
 */

/**
 * @class CameraMatrices
 * @brief **Base container for camera matrices.**
 *
 * Holds projection, view and cached view-projection matrices.
 * Designed to be reused by different camera models.
 */
class CameraMatrices
{
public:
  /** Returns the projection matrix. */
  const glm::mat4 &getProjectionMatrix() const;

  /** Returns the view matrix. */
  const glm::mat4 &getViewMatrix() const;

  /** Returns the cached view-projection matrix. */
  const glm::mat4 &getViewProjectionMatrix() const;

  glm::mat4 m_projection;
  glm::mat4 m_view = glm::mat4(1.F);
  glm::mat4 m_viewProjectionCache;
};

/**
 * @class OrthographicMatrices
 * @brief **Orthographic camera matrix bundle.**
 */
class OrthographicMatrices : public CameraMatrices
{
public:
  OrthographicMatrices(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
};

/**
 * @class PerspectiveMatrices
 * @brief **Perspective camera matrix bundle.**
 */
class PerspectiveMatrices : public CameraMatrices
{
public:
  PerspectiveMatrices(glm::mat4 perspectiveMatrix, glm::mat4 viewMatrix);
};

} // namespace pain

/*
 * ============================================================================
 * **ECS Camera Components**
 * ============================================================================
 */

namespace cmp
{

/**
 * @struct CameraResolution
 * @brief **Resolution and aspect ratio container for cameras.**
 */
struct CameraResolution {
  bool m_active = false; ///< Signal as active to the render pipeline
  glm::ivec2 m_resolution;
  float m_aspectRatio;
  reg::Entity m_entity = reg::Entity{-1};
  glm::ivec2 m_screenPosition = glm::ivec2{
      0}; ///< Where in the screen does the camera starts, usually at (0,0)

  /** Returns the current resolution. */
  const glm::ivec2 &getResolution() const;

  /** Updates resolution and derived values. */
  void setResolution(int w, int h);
  /// gets aspectRatio, mainly used on lua binding
  float getAspectRatio() const;
};

/**
 * @struct OrthoCamera
 * @brief **Orthographic camera ECS component.**
 *
 * Used for 2D rendering and editor-style projections.
 */
struct OrthoCamera : CameraResolution {
  using tag = pain::tag::OrthoCamera;
  float m_zoomLevel = 1.0F;
  pain::OrthographicMatrices m_matrices;

  /** Returns the cached view-projection matrix. */
  const glm::mat4 &getViewProjectionMatrix() const;

  /** Creates a new orthographic camera component. */
  static OrthoCamera create(bool active, int resWidth, int resHeight,
                            float zoomLevel, reg::Entity entity);

  /** Recomputes the view matrix from position and rotation. */
  void recalculateViewMatrix(const glm::vec2 &position, float angleRadians);

  /** Sets projection explicitly using screen bounds. */
  void setProjection(float left, float right, float bottom, float top);

  /** Sets projection using aspect ratio and zoom level. */
  void setProjection(float aspectRatio, float zoomLevel);

  /** Sets projection using window dimensions. */
  void setProjection(int width, int height);

  /** Adds to zoom level. */
  void addZoom(float zoom);
  OrthoCamera() = delete;

private:
  OrthoCamera(bool active, float zoomLevel, pain::OrthographicMatrices oc,
              float aspectRatio, int resWidth, int resHeight,
              reg::Entity entity);
};

/**
 * @struct PerspCamera
 * @brief **Perspective camera ECS component.**
 *
 * Used for 3D rendering.
 */
struct PerspCamera : CameraResolution {
  using tag = pain::tag::PerspCamera;
  float m_fieldOfViewDegrees = 90.0F;
  pain::PerspectiveMatrices m_matrices;

  /** Returns the cached view-projection matrix. */
  const glm::mat4 &getViewProjectionMatrix() const;

  /** Creates a new perspective camera component. */
  static PerspCamera create(bool active, int resWidth, int resHeight,
                            float fieldOfViewDegrees, reg::Entity entity,
                            float yaw, float pitch);

  /** Recomputes the view matrix from camera transform. */
  void recalculateViewMatrix(glm::vec3 position, glm::vec3 frontCamera);

  /** Updates the projection matrix. */
  void setProjection(float aspectRatio, float fieldOfViewDegrees);
  void setProjection(int width, int height);

  PerspCamera() = delete;

private:
  PerspCamera(bool active, pain::PerspectiveMatrices pe, float aspectRatio,
              int resWidth, int resHeight, float fieldOfViewDegrees,
              reg::Entity entity);
};

} // namespace cmp

namespace Component = cmp;
