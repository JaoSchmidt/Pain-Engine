#include "GUI/ImGuiDebugMenu.h"

#include "CoreFiles/Application.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "GUI/ImGuiDebugRegistry.h"

namespace pain
{

// TODO:
// - add onImGuiRender inside the NativeScript
// - create dynamic updates inside the mousepointer for testing

void ImGuiDebugMenu::onRender(pain::Renderer2d &renderer, bool isMinimized,
                              DeltaTime currentTime)
{
  UNUSED(renderer)
  UNUSED(isMinimized)
  UNUSED(currentTime)
  ImGui::Begin("Debug info");
  auto &camCC =
      getScene().getComponent<pain::OrthoCameraComponent>(m_cameraEntity);

  ImGui::Text("Camera zoom: %.3f", camCC.m_zoomLevel);
  ImGuiDebugRegistry::renderAll();
  ImGui::End();
}

ImGuiDebugMenu::ImGuiDebugMenu(reg::Entity entity, pain::Scene &scene,
                               pain::Application *app, reg::Entity cameraEntity,
                               reg::Entity mp)
    : ExtendedEntity(entity, scene), m_cameraEntity(cameraEntity),
      m_mousePointerEntity(mp), m_app(app) {};

ImGuiDebugMenu::ImGuiDebugMenu(ImGuiDebugMenu &&other) noexcept
    : ExtendedEntity(std::move(other)), m_cameraEntity(other.m_cameraEntity),
      m_mousePointerEntity(other.m_mousePointerEntity),
      m_app(std::exchange(other.m_app, nullptr)) {};

ImGuiDebugMenu &ImGuiDebugMenu::operator=(ImGuiDebugMenu &&other) noexcept
{
  if (this != &other) {
    ExtendedEntity::operator=(std::move(other));
    m_app = std::exchange(other.m_app, nullptr);
    m_cameraEntity = other.m_cameraEntity;
    m_mousePointerEntity = other.m_mousePointerEntity;
  }
  return *this;
}

} // namespace pain
