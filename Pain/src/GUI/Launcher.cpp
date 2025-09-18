#include "GUI/Launcher.h"
#include "Assets/IniWrapper.h"
#include "Assets/ResourceManager.h"
#include "Core.h"
#include "CoreFiles/Application.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"
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
  ImGuiLauncher(Entity entity, Bitmask bitmask, Scene &scene, Application *app)

      : ExtendedEntity(entity, bitmask, scene), m_init(), m_app(app) {};
  ~ImGuiLauncher() = default;
  NONCOPYABLE(ImGuiLauncher);
  ImGuiLauncher(ImGuiLauncher &&other) noexcept
      : ExtendedEntity(std::move(other)),
        m_availableResolutions(std::move(other.m_availableResolutions)),
        m_windowFlags(std::exchange(other.m_windowFlags, 0)),
        m_dockspaceOpen(std::exchange(other.m_dockspaceOpen, true)),
        m_init(std::move(other.m_init)),
        m_app(std::exchange(other.m_app, nullptr)) {};
  ImGuiLauncher &operator=(ImGuiLauncher &&other) noexcept
  {
    if (this != &other) {
      ExtendedEntity::operator=(std::move(other));
      m_availableResolutions = std::move(other.m_availableResolutions);
      m_windowFlags = std::exchange(other.m_windowFlags, 0);
      m_dockspaceOpen = std::exchange(other.m_dockspaceOpen, true);
      m_init = std::move(other.m_init);
      m_app = std::exchange(other.m_app, nullptr);
    }
    return *this;
  }

  // void init(Application *app) { m_app = app; }
  void onCreate()
  {
    if (resources::exists_file(m_initFilename)) {
      mINI::INIFile file(m_initFilename);
      mINI::INIStructure ini;
      file.read(ini);
      // clang-format off
      m_init.hideConfig.value = IniWrapper::getBoolean(
          ini, "settings", m_init.hideConfig.name, m_init.hideConfig.getDefault());
      m_init.fullscreen.value = IniWrapper::getBoolean(
          ini, "settings", m_init.fullscreen.name, m_init.fullscreen.getDefault());
      m_init.assetsPath.value = IniWrapper::get(
          ini, "settings", m_init.assetsPath.name, m_init.assetsPath.getDefault());
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
      updateIni();
      m_app->stopLoop();
    }

    ImGui::SameLine(0.0f, spacing);

    if (ImGui::Button("Play", ImVec2(buttonWidth, buttonHeight))) {
      updateIni();
      m_app->stopLoop(true);
    }

    ImGui::End();
  }

private:
  void updateIni()
  {
    mINI::INIFile file(m_initFilename);
    mINI::INIStructure ini;
    file.read(ini);
    ini["settings"][m_init.hideConfig.name] =
        m_init.hideConfig.value ? "true" : "false";
    ini["settings"][m_init.fullscreen.name] =
        m_init.fullscreen.value ? "true" : "false";
    ini["settings"][m_init.assetsPath.name] = m_init.assetsPath.value;
    file.write(ini);
  }
  constexpr static const char *m_initFilename = "config.ini";
  std::vector<std::string> m_availableResolutions;

  ImGuiWindowFlags m_windowFlags =
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;
  bool m_dockspaceOpen = true;
  IniConfig m_init;
  Application *m_app = nullptr;
};

<<<<<<< HEAD
// honestly didn't think this through, but I guess I need a dummy
class ImGuiDummy : public NormalEntity<ImGuiComponent>
{
public:
  ImGuiDummy(Scene *scene, Application *app) : NormalEntity(*scene)
  {
    createComponents(*scene, ImGuiComponent{});
    withImGuiScript<ImGuiLauncher>(*scene, app);
  }
};

=======
>>>>>>> lua_script
class SettingsScene : public Scene
{
public:
  using Scene::Scene;

  SettingsScene(void *sdlcontext, SDL_Window *window, sol::state &solState,
                float aspectRatio, float zoom, Application *app)
<<<<<<< HEAD
      : Scene(sdlcontext, window, solState)
=======
      : Scene(sdlcontext, window, solState, ImGuiComponent{})
>>>>>>> lua_script
  {
    m_orthocamera = std::make_unique<OrthoCamera>(this, aspectRatio, zoom);
    app->setRendererCamera(*(std::as_const(m_orthocamera)
                                 ->getComponent<OrthoCameraComponent>(*this)
                                 .m_camera));
    m_orthocamera->withScript<OrthoCameraScript>(*this);
<<<<<<< HEAD
    m_imguiDummy = std::make_unique<ImGuiDummy>(this, app);
=======

    withImGuiScript<ImGuiLauncher>(app);
>>>>>>> lua_script
  };

private:
  std::unique_ptr<OrthoCamera> m_orthocamera;
<<<<<<< HEAD
  // std::unique_ptr<ImGuiLauncher> m_imgui;
  std::unique_ptr<ImGuiDummy> m_imguiDummy;
=======
>>>>>>> lua_script
};

Application *createLauncher()
{
  PLOG_T("Opening Settings App");
  const char *title = "Settings";
  const int width = 500;
  const int height = 200;
  Application *settingsApp =
      Application::createApplication(title, width, height, true);
  auto pscene = settingsApp->createSceneUPtr<SettingsScene>(
      (float)width / height, 1.0f, settingsApp);

  settingsApp->pushScene("settingsMain", std::move(pscene));
  settingsApp->attachScene("settingsMain");
  return settingsApp;
}
} // namespace pain
