#include "CameraController.h"

void CameraController::onEvent(const SDL_Event &event)
{
  pain::OrthoCameraComponent &cc = getComponent<pain::OrthoCameraComponent>();
  if (event.type == SDL_WINDOWEVENT) {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
      onWindowResized(event, cc);
    }
  }
}

void CameraController::onWindowResized(const SDL_Event &event,
                                       pain::OrthoCameraComponent &cc)
{
  pain::Renderer2d::setViewport(0, 0, event.window.data1, event.window.data2);
  cc.m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  cc.m_camera->SetProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                             cc.m_aspectRatio * cc.m_zoomLevel, -cc.m_zoomLevel,
                             cc.m_zoomLevel);
}
