#pragma once
#include "pch.h"

#include "Core.h"
#include "ECS/Systems.h"

#include "imgui.h"

namespace pain
{

class ImGuiInstance
{
public:
  virtual void onImGuiUpdate() = 0;
  virtual ~ImGuiInstance() {};
};
namespace Systems
{
class ImGui : public System
{
public:
  // HACK: The singleton (static) approach assumes that there
  // will be only one context necessary for Dear ImGui to work.
  // Keep an eye on that in case it doesn't work.
  ImGui(ArcheRegistry &archetype, void *context, SDL_Window *window,
        ImGuiConfigFlags flags =
            ImGuiConfigFlags_NavEnableKeyboard  // Enable Keyboard Controls
            | ImGuiConfigFlags_NavEnableGamepad // Enable Gamepad Controls
            | ImGuiConfigFlags_DockingEnable    // Enable Docking
            |
            ImGuiConfigFlags_ViewportsEnable // Enable Multi-Viewport / Platform
  );
  ~ImGui();
  void onRender(bool isMinimized, double currentTime);
  void onEvent(const SDL_Event &e);

private:
  ImGuiIO *m_io = nullptr;
  bool m_showAnotherWindow;
  ImVec4 m_clearColor;
  ImGuiDockNodeFlags m_dockspace_flags = ImGuiDockNodeFlags_None;
};

} // namespace Systems
} // namespace pain
