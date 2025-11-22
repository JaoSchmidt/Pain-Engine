
#include "CoreFiles/Application.h"
#include "ECS/Scriptable.h"
#include <imgui.h>

namespace pain
{

class ImGuiDebugMenu : public pain::ExtendedEntity
{
public:
  void onRender(pain::Renderer2d &renderer, bool isMinimized,
                double currentTime);
  ImGuiDebugMenu(reg::Entity entity, pain::Scene &scene, pain::Application *app,
                 reg::Entity cameraEntity, reg::Entity mp);
  ImGuiDebugMenu(ImGuiDebugMenu &&other) noexcept;
  ImGuiDebugMenu &operator=(ImGuiDebugMenu &&other) noexcept;

private:
  reg::Entity m_cameraEntity = reg::Entity{-2};
  reg::Entity m_mousePointerEntity = reg::Entity{-2};
  glm::ivec2 m_mousePos;
  pain::Application *m_app = nullptr;
};

} // namespace pain
