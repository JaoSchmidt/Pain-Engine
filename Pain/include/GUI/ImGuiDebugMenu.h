
#include "CoreFiles/Application.h"
#include "ECS/Scriptable.h"
#include <imgui.h>

namespace pain
{

class ImGuiDebugMenu : public pain::WorldObject
{
public:
  void onRender(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime);
  ImGuiDebugMenu(reg::Entity entity, pain::Scene &scene, pain::Application *app,
                 reg::Entity cameraEntity, reg::Entity mp);
  ImGuiDebugMenu(ImGuiDebugMenu &&other) noexcept;
  ImGuiDebugMenu &operator=(ImGuiDebugMenu &&other) = delete;

private:
  reg::Entity m_cameraEntity = reg::Entity{-2};
  reg::Entity m_mousePointerEntity = reg::Entity{-2};
  pain::Application *m_app = nullptr;
};

} // namespace pain
