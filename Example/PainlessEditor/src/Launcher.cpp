/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Launcher.h"
#include "Assets/ManagerIni.h"
#include "Core.h"
#include "CoreFiles/Application.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/CameraComponent.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Scriptable.h"
#include "ECS/UIScene.h"
#include "ImGuiComponent.h"
#include "ImGuiEmplacer.h"
#include "ImGuiSys.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <cstdio>
#include <pain.h>
#include <stdio.h> /* defines FILENAME_MAX */

namespace painless
{

class ImGuiLauncher : public pain::UIObject
{
public:
  ImGuiLauncher(reg::Entity entity, pain::UIScene &scene,
                pain::Application *app)
      : pain::UIObject(entity, scene), m_init(), m_app(app) {};
  std::vector<std::string> m_availableResolutions;

  ~ImGuiLauncher() = default;
  NONCOPYABLE(ImGuiLauncher);
  ImGuiLauncher(ImGuiLauncher &&other) noexcept
      : pain::UIObject(std::move(other)),
        m_availableResolutions(std::move(other.m_availableResolutions)),
        m_windowFlags(std::exchange(other.m_windowFlags, 0)),
        m_dockspaceOpen(std::exchange(other.m_dockspaceOpen, true)),
        m_init(std::move(other.m_init)),
        m_app(std::exchange(other.m_app, nullptr)) {};
  ImGuiLauncher &operator=(ImGuiLauncher &&other) = delete;

  // void init(Application *app) { m_app = app; }
  void onCreate()
  {
    ELOG_I("Creating Launcher");
    m_init.readAndUpdate(true);
  }
  void onRender(pain::RenderApi &renderer, 
                pain::DeltaTime deltaTime)
  {
    UNUSED(renderer)
    UNUSED(deltaTime)
    
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0F, 20.0F));
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
    ImGui::Dummy(ImVec2(0.0F, 10.0F)); // vertical spacing from separator

    float buttonWidth = 120.0F;
    float buttonHeight = 0.0F; // auto height
    float spacing = 10.0F;     // space between buttons

    // total width of both buttons + spacing
    float totalWidth = buttonWidth * 2 + spacing;

    // right align: move cursor so that the block ends at window's right edge
    float windowWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetCursorPosX(windowWidth - totalWidth);

    if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
      m_init.write(pain::AppInit::configIniFile);
      m_app->stopLoop();
    }

    ImGui::SameLine(0.0F, spacing);

    if (ImGui::Button("Play", ImVec2(buttonWidth, buttonHeight))) {
      m_init.write(pain::AppInit::configIniFile);
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
  pain::IniConfig m_init;
  pain::Application *m_app = nullptr;
};

pain::Application *createLauncher()
{
  logWrapper::s_initEditorLogger();
  ELOG_T("Opening Settings App");
  const char *title = "Settings";
  const int width = 500;
  const int height = 200;

  pain::Application *settingsApp = pain::Application::createApplication(
      {.title = title, .defaultWidth = width, .defaultHeight = height},
      {.swapChainTarget = true});
  pain::Scene &scene = settingsApp->getWorldScene();
  settingsApp->getRenderApi().m_renderer2d.setCellGridSize(0);
  settingsApp->setFocusedOrHovered(false);
  // creates a simple and dumb camera to avoid any
  // assert later. This makes no difference in release
  scene.createComponents(                              //
      scene.getEntity(), pain::Transform2dComponent{}, //
      Component::OrthoCamera::create(true, width, height, 5.F,
                                     scene.getEntity()) //
  );
  pain::UIScene &uiscene = settingsApp->createUIScene();
  uiscene.addSystem<pain::Systems::ImGuiSys>(settingsApp->getRenderContext(),
                                             settingsApp->getRenderWindow());
  uiscene.createComponents(uiscene.getEntity(), painless::ImGuiComponent{});
  painless::emplaceImGuiScript<ImGuiLauncher>(uiscene.getEntity(), uiscene,
                                              settingsApp);

  return settingsApp;
}
} // namespace painless
