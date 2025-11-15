#include "GUI/Launcher.h"
#include "Assets/IniWrapper.h"
#include "Assets/ResourceManager.h"
#include "Core.h"
#include "CoreFiles/Application.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Scene.h"
#include "ECS/Scriptable.h"
#include "GUI/ImGuiComponent.h"
#include "Misc/BasicOrthoCamera.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <cstdio>
#include <stdio.h> /* defines FILENAME_MAX */

namespace pain
{

class ImGuiLauncher : public ExtendedEntity
{
public:
  static pain::Scene &createScriptScene(int resWeight, int resHeight,
                                        float zoom,
                                        pain::Application *settingsApp)
  {
    pain::Scene &scene =
        settingsApp->createScene(1.f, pain::NativeScriptComponent{});
    pain::OrthoCamera orthoCamera = {&scene, resWeight, resHeight, zoom};

    settingsApp->setRendererCamera(
        *(std::as_const(orthoCamera)
              .getComponent<OrthoCameraComponent>(scene)
              .m_matrices),
        orthoCamera.getEntity());
    orthoCamera.withScript<OrthoCameraScript>(scene);

    scene.withImGuiScript<ImGuiLauncher>(ImGuiLauncher(
        scene.getEntity(), scene, settingsApp, std::move(orthoCamera)));
    return scene;
  }
  ~ImGuiLauncher() = default;
  NONCOPYABLE(ImGuiLauncher);
  ImGuiLauncher(ImGuiLauncher &&other) noexcept
      : ExtendedEntity(std::move(other)),
        m_availableResolutions(std::move(other.m_availableResolutions)),
        m_windowFlags(std::exchange(other.m_windowFlags, 0)),
        m_dockspaceOpen(std::exchange(other.m_dockspaceOpen, true)),
        m_init(std::move(other.m_init)),
        m_app(std::exchange(other.m_app, nullptr)),
        m_orthocamera(std::move(other.m_orthocamera)) {};
  ImGuiLauncher &operator=(ImGuiLauncher &&other) noexcept
  {
    if (this != &other) {
      ExtendedEntity::operator=(std::move(other));
      m_availableResolutions = std::move(other.m_availableResolutions);
      m_windowFlags = std::exchange(other.m_windowFlags, 0);
      m_dockspaceOpen = std::exchange(other.m_dockspaceOpen, true);
      m_init = std::move(other.m_init);
      m_app = std::exchange(other.m_app, nullptr);
      m_orthocamera = std::move(other.m_orthocamera);
    }
    return *this;
  }

  // void init(Application *app) { m_app = app; }
  void onCreate()
  {
    if (resources::exists_file(m_app->configIniFile)) {
      m_init.readAndUpdate(m_app->configIniFile.c_str());
    }
    // clang-format on
  }
  void onRender(Renderer2d &renderer, bool isMinimized, double currentTime)
  {
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

    const char *items[] = {"1920x1080", "1680x1050", "1400x1050",
                           "1600x900",  "1280x1024", "1400x900",
                           "1280x960",  "800x600",   "640x480"};
    static const char *current_item = NULL;

    if (ImGui::BeginCombo("##combo", "Resolution"))
    // The second parameter is the label previewed before opening the combo.
    {
      for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
        bool is_selected = (current_item == items[n]);
        // You can store your selection however you want, outside or inside your
        // objects
        if (ImGui::Selectable(items[n], is_selected)) {
          current_item = items[n];
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
      m_init.write(m_app->configIniFile.c_str());
      m_app->stopLoop();
    }

    ImGui::SameLine(0.0f, spacing);

    if (ImGui::Button("Play", ImVec2(buttonWidth, buttonHeight))) {
      m_init.write(m_app->configIniFile.c_str());
      m_app->stopLoop(true);
    }

    ImGui::End();
  }

private:
  ImGuiLauncher(reg::Entity entity, Scene &scene, Application *app,
                OrthoCamera &&camera)
      : ExtendedEntity(entity, scene), m_init(), m_app(app),
        m_orthocamera(std::move(camera)) {};
  std::vector<std::string> m_availableResolutions;

  ImGuiWindowFlags m_windowFlags =
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;
  bool m_dockspaceOpen = true;
  IniConfig m_init;
  Application *m_app = nullptr;
  OrthoCamera m_orthocamera;
};

Application *createLauncher()
{
  PLOG_T("Opening Settings App");
  const char *title = "Settings";
  const int width = 500;
  const int height = 200;
  const float zoom = 1.f;
  Application *settingsApp =
      Application::createApplication(title, width, height, true);
  ImGuiLauncher::createScriptScene(width, height, zoom, settingsApp);

  return settingsApp;
}
} // namespace pain
