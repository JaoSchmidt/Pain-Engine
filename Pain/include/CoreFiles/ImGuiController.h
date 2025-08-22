#pragma once
#include "pch.h"

#include "Core.h"

#include "imgui.h"

namespace pain
{

class ImGuiInstance
{
public:
  virtual void onImGuiUpdate() = 0;
  virtual ~ImGuiInstance() {};
};
class ImGuiController
{
public:
  // HACK: The singleton (static) approach assumes that there
  // will be only one context necessary for Dear ImGui to work.
  // Keep an eye on that in case it doesn't work.
  ImGuiController(
      void *context, SDL_Window *window,
      ImGuiConfigFlags flags =
          ImGuiConfigFlags_NavEnableKeyboard  // Enable Keyboard Controls
          | ImGuiConfigFlags_NavEnableGamepad // Enable Gamepad Controls
          | ImGuiConfigFlags_DockingEnable    // Enable Docking
          | ImGuiConfigFlags_ViewportsEnable // Enable Multi-Viewport / Platform
  );
  ~ImGuiController();
  void onRender();
  void onMinimizeRender();
  void onUpdate(bool isMinimized);
  void onEvent(const SDL_Event &e);

  void addImGuiMenu(ImGuiInstance *imGuiInstance)
  {
    m_imguiInstances.push_back(imGuiInstance);
  }

private:
  ImGuiIO *m_io;
  bool m_showAnotherWindow;
  ImVec4 m_clearColor;
  std::vector<ImGuiInstance *> m_imguiInstances = {};
  bool m_dockspaceEnable = false;
  ImGuiDockNodeFlags m_dockspace_flags = ImGuiDockNodeFlags_None;
};

} // namespace pain
