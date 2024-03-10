#pragma once
#include "pch.gch"

#include "Core.h"

namespace pain
{

class EXPORT Camera
{
public:
  void SetPosition(const glm::vec3 &position)
  {
    m_Position = position;
    RecalculateViewMatrix();
  }

  const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
  const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
  const glm::mat4 &GetViewProjectionMatrix() const
  {
    return m_ViewProjectionCacheMatrix;
  }

protected:
  virtual void RecalculateViewMatrix() = 0;
  const glm::vec3 &GetPosition() const { return m_Position; }
  glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
  glm::mat4 m_ProjectionMatrix;
  glm::mat4 m_ViewMatrix;
  glm::mat4 m_ViewProjectionCacheMatrix;
};

class OrthographicCamera : public Camera
{
public:
  OrthographicCamera(float left, float right, float bottom, float top);

  float GetRotation() const { return m_Rotation; }
  void SetRotation(float rotation)
  {
    m_Rotation = rotation;
    RecalculateViewMatrix();
  }

  ~OrthographicCamera() {}

protected:
  void RecalculateViewMatrix() override;

private:
  float m_Rotation = 0.0f;
};

} // namespace pain
