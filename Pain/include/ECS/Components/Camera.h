#pragma once

#include "CoreRender/Camera.h"
#include "pch.h"

namespace pain
{

struct OrthoCameraComponent {
  float m_aspectRatio = 800.0 / 600.0;
  float m_zoomLevel = 1.0f;
  OrthographicMatrices *m_camera = nullptr;
  OrthoCameraComponent() = default;
  OrthoCameraComponent(float aspectRatio, float zoomLevel)
      : m_aspectRatio(aspectRatio), m_zoomLevel(zoomLevel)
  {
    m_camera = new OrthographicMatrices(-m_aspectRatio * m_zoomLevel,
                                        m_aspectRatio * m_zoomLevel,
                                        -m_zoomLevel, m_zoomLevel);
  }
  ~OrthoCameraComponent() { delete m_camera; }
  OrthoCameraComponent(const OrthoCameraComponent &other)
      : m_aspectRatio(other.m_aspectRatio), m_zoomLevel(other.m_zoomLevel)
  {
    if (other.m_camera) {
      m_camera = new OrthographicMatrices(*other.m_camera);
    }
  }
  OrthoCameraComponent &operator=(const OrthoCameraComponent &other)
  {
    if (this == &other) {
      return *this;
    }
    m_aspectRatio = other.m_aspectRatio;
    m_zoomLevel = other.m_zoomLevel;
    delete m_camera;
    if (other.m_camera) {
      m_camera = new OrthographicMatrices(*other.m_camera);
    } else {
      m_camera = nullptr;
    }
    return *this;
  }
};

} // namespace pain
