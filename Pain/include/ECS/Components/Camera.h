#pragma once

#include "CoreRender/Camera.h"
#include "pch.h"

namespace pain
{

struct OrthoCameraComponent {
  float m_aspectRatio = 800.0 / 600.0;
  float m_zoomLevel = 1.0f;
  OrthographicMatrices *m_matrices = nullptr;
  OrthoCameraComponent() = default;
  OrthoCameraComponent(float aspectRatio, float zoomLevel)
      : m_aspectRatio(aspectRatio), m_zoomLevel(zoomLevel)
  {
    m_matrices = new OrthographicMatrices(-m_aspectRatio * m_zoomLevel,
                                          m_aspectRatio * m_zoomLevel,
                                          -m_zoomLevel, m_zoomLevel);
  }
  ~OrthoCameraComponent() { delete m_matrices; }
  OrthoCameraComponent(const OrthoCameraComponent &other)
      : m_aspectRatio(other.m_aspectRatio), m_zoomLevel(other.m_zoomLevel)
  {
    if (other.m_matrices) {
      m_matrices = new OrthographicMatrices(*other.m_matrices);
    }
  }
  OrthoCameraComponent &operator=(const OrthoCameraComponent &other)
  {
    if (this == &other) {
      return *this;
    }
    m_aspectRatio = other.m_aspectRatio;
    m_zoomLevel = other.m_zoomLevel;
    delete m_matrices;
    if (other.m_matrices) {
      m_matrices = new OrthographicMatrices(*other.m_matrices);
    } else {
      m_matrices = nullptr;
    }
    return *this;
  }
};

} // namespace pain
