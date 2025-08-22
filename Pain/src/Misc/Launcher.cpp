#include "Misc/Launcher.h"
#include "CoreFiles/Application.h"
#include "CoreFiles/ImGuiController.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Scene.h"
#include "imgui.h"

namespace pain
{

class ImGuiSettings : public pain::ImGuiInstance
{
public:
  ImGuiSettings(Application &app) : m_app(app) {}
  void onImGuiUpdate() override
  {
    ImGuiWindowFlags m_windowFlags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (m_optFullscreen) {
      ImGuiViewport *viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->Pos);
      ImGui::SetNextWindowSize(viewport->Size);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      m_windowFlags |= ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
      m_windowFlags |=
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (m_dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
      m_windowFlags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window
    // is collapsed). This is because we want to keep our DockSpace() active. If
    // a DockSpace() is inactive, all active windows docked into it will lose
    // their parent and become undocked. We cannot preserve the docking
    // relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in
    // limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &m_dockspaceOpen, m_windowFlags);
    ImGui::PopStyleVar();

    if (m_optFullscreen)
      ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_dockspaceFlags);
    }

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        // Disabling fullscreen would allow the window to be moved to the front
        // of other windows, which we can't undo at the moment without finer
        // window depth/z control.
        // ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

        if (ImGui::MenuItem("Exit"))
          m_app.stop();
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }

    ImGui::Begin("Settings", nullptr, windowFlags);
    ImGui::Text("Renderer2D Stats?");
    ImGui::End();

    ImGui::End();
  }

private:
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
  // ImGuiWindowFlags windowFlags =
  //     ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  ImGuiDockNodeFlags m_dockspaceFlags = ImGuiDockNodeFlags_None;
  bool m_optFullscreen = true;
  bool m_dockspaceOpen = true;
  Application &m_app;
};

class SettingsScene : public Scene
{
public:
  SettingsScene(sol::state &luaState) : Scene(luaState) {}
  void init(Application &app, Scene *scene, float aspectRatio, float zoom)
  {
    m_orthocamera = std::make_unique<OrthoCamera>(scene, aspectRatio, zoom);
    Renderer2d::init(*m_orthocamera);
    pain::NativeScriptComponent &a =
        m_orthocamera->getComponent<pain::NativeScriptComponent>(*scene);
    a.bind<pain::OrthoCameraController>();
    scene->initializeScript(scene, a, m_orthocamera->getEntity(),
                            m_orthocamera->getBitMask());
    m_imgui = std::make_unique<ImGuiSettings>(app);
    app.addImGuiInstance(m_imgui.get());
  }
  // ~SettingsScene() { delete m_imgui; }
  // NONCOPYABLE(SettingsScene);
  // SettingsScene &operator=(SettingsScene &&o) noexcept
  // {
  //   m_imgui = o.m_imgui;
  //   return *this;
  // };
  // ~SettingsScene() override = default;

private:
  void onUpdate(double deltaTime) override {}
  void onRender(double currentTime) override {}
  void onEvent(const SDL_Event &event) override {}
  std::unique_ptr<OrthoCamera> m_orthocamera;
  std::unique_ptr<ImGuiSettings> m_imgui;
};

Application *createLauncher()
{
  PLOG_T("Opening Settings App");
  const char *title = "Settings";
  const int width = 500;
  const int height = 500;
  Application *settingsApp = new Application(title, width, height, true);
  Scene *scene = new SettingsScene(settingsApp->getLuaState());
  ((SettingsScene *)scene)
      ->init(*settingsApp, scene, (float)width / height, 1.0f);

  settingsApp->pushScene("settingsMain", scene);
  settingsApp->attachScene("settingsMain");
  return settingsApp;
}
} // namespace pain
