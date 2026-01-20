/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "GUI/Launcher.h"
#include "Assets/ManagerFile.h"
#include "Assets/ManagerIni.h"
#include "Core.h"
#include "CoreFiles/Application.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Scriptable.h"
#include "ECS/UIScene.h"
#include "Misc/BasicOrthoCamera.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <cstdio>
#include <stdio.h> /* defines FILENAME_MAX */

namespace pain
{

class ImGuiLauncher : public UIObject
{
public:
  ImGuiLauncher(reg::Entity entity, UIScene &scene, Application *app)
      : UIObject(entity, scene), m_init(), m_app(app) {};
  std::vector<std::string> m_availableResolutions;

  ~ImGuiLauncher() = default;
  NONCOPYABLE(ImGuiLauncher);
  ImGuiLauncher(ImGuiLauncher &&other) noexcept
      : UIObject(std::move(other)),
        m_availableResolutions(std::move(other.m_availableResolutions)),
        m_windowFlags(std::exchange(other.m_windowFlags, 0)),
        m_dockspaceOpen(std::exchange(other.m_dockspaceOpen, true)),
        m_init(std::move(other.m_init)),
        m_app(std::exchange(other.m_app, nullptr)) {};
  ImGuiLauncher &operator=(ImGuiLauncher &&other) = delete;

  // void init(Application *app) { m_app = app; }
  void onCreate()
  {
    PLOG_I("Creating Launcher");
    m_init.readAndUpdate(true);
  }
  void onRender(Renderer2d &renderer, bool isMinimized, DeltaTime deltaTime)
  {
    UNUSED(renderer)
    UNUSED(deltaTime)
    UNUSED(isMinimized)
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    ImGui::Begin("DockSpace Demo", &m_dockspaceOpen, m_windowFlags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    static constexpr std::array<const char *, 9> items = {
        "1920x1080", "1680x1050", "1400x1050", "1600x900", "1280x1024",
        "1400x900",  "1280x960",  "800x600",   "640x480"};
    static const char *currentItem = items[items.size() - 2];

    if (ImGui::BeginCombo("##combo", "Resolution"))
    // The second parameter is the label previewed before opening the combo.
    {
      for (size_t n = 0; n < items.size(); n++) {
        bool is_selected = (currentItem == items[n]);
        // You can store your selection however you want, outside or inside your
        // objects
        if (ImGui::Selectable(items[n], is_selected)) {
          currentItem = items[n];
          std::sscanf(currentItem, "%dx%d", &m_init.defaultWidth.value,
                      &m_init.defaultHeight.value);
        }
        if (is_selected)
          ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                        // opening the combo (scrolling + for
                                        // keyboard navigation support)
      }
      ImGui::EndCombo();
    }

    // Fullscreen checkbox
    ImGui::Checkbox("Fullscreen", &m_init.fullscreen.value);

    // Hide next time
    ImGui::Checkbox("Hide this config next time", &m_init.hideConfig.value);

    // Advanced section
    if (ImGui::CollapsingHeader("Advanced Options")) {
      ImGui::InputText("Assets Path", &m_init.assetsPath.value);
    }

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // vertical spacing from separator

    float buttonWidth = 120.0f;
    float buttonHeight = 0.0f; // auto height
    float spacing = 10.0f;     // space between buttons

    // total width of both buttons + spacing
    float totalWidth = buttonWidth * 2 + spacing;

    // right align: move cursor so that the block ends at window's right edge
    float windowWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(windowWidth - totalWidth);

    if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
      m_init.write(AppContext::configIniFile);
      m_app->stopLoop();
    }

    ImGui::SameLine(0.0f, spacing);

    if (ImGui::Button("Play", ImVec2(buttonWidth, buttonHeight))) {
      m_init.write(AppContext::configIniFile);
      m_app->stopLoop(true);
    }

    ImGui::End();
  }

private:
  ImGuiWindowFlags m_windowFlags =
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;
  bool m_dockspaceOpen = true;
  IniConfig m_init;
  Application *m_app = nullptr;
};

Application *createLauncher()
{
  PLOG_T("Opening Settings App");
  const char *title = "Settings";
  const int width = 500;
  const int height = 200;

  Application *settingsApp = Application::createApplication(
      {.title = title, .defaultWidth = width, .defaultHeight = height},
      {.swapChainTarget = true});
  pain::UIScene &uiscene = settingsApp->createUIScene(pain::ImGuiComponent{});
  UIScene::emplaceImGuiScript<ImGuiLauncher>(uiscene.getEntity(), uiscene,
                                             settingsApp);

  return settingsApp;
}
} // namespace pain
