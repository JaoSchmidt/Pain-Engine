#include <pain.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

#include "Editor.h"

class MainScript : public pain::WorldObject {
 public:
  static MainScript &createScriptScene(pain::Scene &scene, int cameraWidth,
                                       int cameraHeight, float zoomOrFOV,
                                       pain::Application *app) {
    UNUSED(cameraWidth);
    UNUSED(cameraHeight);
    UNUSED(app);
    UNUSED(zoomOrFOV);
    return pain::Scene::emplaceScript<MainScript>(scene.getEntity(), scene);
  }
  void onCreate() {}
  MainScript(reg::Entity entity, pain::Scene &scene)
      : pain::WorldObject(entity, scene) {};

  void onUpdate(pain::DeltaTime dt) { UNUSED(dt); }
  void onRender(pain::Renderers &rs, bool isMinimazed,
                pain::DeltaTime currentTime) {
    UNUSED(isMinimazed)
    rs.renderer2d.drawQuad({-0.2f, -0.2f}, {0.25f, 0.25f}, {204, 51, 25, 255},
                           glm::pi<float>() * -currentTime.getSeconds(),
                           pain::RenderLayer::Default,
                           pain::TextureManager::getTexture(
                               "resources/textures/Checkerboard.png"));
    rs.renderer2d.drawQuad({0.0f, 0.0f}, {0.25f, 0.25f}, {230, 230, 51, 255},
                           pain::RenderLayer::Default,
                           pain::TextureManager::getDefaultTexture(
                               pain::TextureManager::General, false));

    rs.renderer2d.drawQuad({-0.5f, 0.5f}, {0.25f, 0.25f}, {255, 255, 255, 255},
                           pain::RenderLayer::Default,
                           pain::TextureManager::getTexture(
                               "resources/textures/Checkerboard original.png"));
    rs.renderer2d.drawQuad({0.2f, -0.2f}, {0.25f, 0.25f}, {204, 51, 26, 255},
                           glm::pi<float>() * currentTime.getSeconds(),
                           pain::RenderLayer::MuchCloser,
                           pain::TextureManager::getTexture(
                               "resources/textures/Checkerboard.png"));
    rs.renderer2d.drawQuad({0.2f, 0.2f}, {0.25f, 0.25f}, {230, 230, 51, 255},
                           pain::RenderLayer::MuchCloser,
                           pain::TextureManager::getDefaultTexture(
                               pain::TextureManager::General, false));
    rs.renderer2d.drawQuad({-0.2f, 0.2f}, {0.25f, 0.25f}, {255, 255, 255, 255},
                           pain::RenderLayer::MuchCloser,
                           pain::TextureManager::getTexture(
                               "resources/textures/Checkerboard original.png"));
    rs.renderer2d.drawCircle({0.5f, 0.5f}, 0.25f, {51, 75, 230, 255});
    rs.renderer2d.drawTri({-0.5f, -0.5f}, {0.25f, 0.25f},
                          {0.2f, 0.3f, 0.9f, 1.f});
  }
};

pain::Application *pain::createApplication() {
  // Retrieve the context the player will alter when using the launcher
  IniConfig ini;
  ini.readAndUpdate();

  // Retrieve the app context defined inside "resources/InternalConfig.ini"
  InternalConfig internalIni;
  internalIni.readAndUpdate(ini.assetsPath.value.c_str());

  // Create the application + OpenGL + Event contexts
  Application *app = Application::createApplication(  //
      {.title = internalIni.title.get().c_str(),      //
       .defaultWidth = ini.defaultWidth.get(),        //
       .defaultHeight = ini.defaultHeight.get(),
       .is3d = internalIni.is3d.get()},                       //
      {.swapChainTarget = internalIni.swapChainTarget.get()}  //
  );

  // Create the ECS World Scene
  pain::Scene &scene = app->createWorldSceneComponents(
      internalIni.gridSize.get(), pain::NativeScriptComponent{});

  // Individually add each system
  scene.addSystem<Systems::SweepAndPruneSys>();
  scene.addSystem<Systems::Render>();
  scene.addSystem<Systems::NativeScript>();
  scene.addSystem<Systems::LuaScript>();
  scene.addSystem<Systems::Kinematics>();
  scene.addSystem<Systems::LuaSchedulerSys>();

  // (Optional) Defining a small native script (MainScript) for the world scene
  // that will be executed on. Must have added System::NativeScript
  MainScript::createScriptScene(      //
      scene, ini.defaultWidth.get(),  //
      ini.defaultHeight.get(),        //
      internalIni.zoomLevel.get(),    //
      app                             //
  );

  // (Optional) Creating the ECS UI scene
  UIScene &uiScene = app->createUIScene(pain::ImGuiComponent{});
  // (Optional) A small native script that works as our game engine editor
  UIScene::emplaceImGuiScript<PainlessEditor>(uiScene.getEntity(), uiScene,
                                              *app);

  return app;
}

#ifdef PLATFORM_IS_LINUX
int main(int argc, char *argv[])
#elif defined PLATFORM_IS_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine,
                   int nCmdShow)
#endif
{
  bool isSettingsGuiNeeded = pain::Pain::initiateIni();
  EndGameFlags flags;
  flags.restartGame = !isSettingsGuiNeeded;
  if (isSettingsGuiNeeded) {
    pain::Application *app = pain::createLauncher();
    flags = pain::Pain::runAndDeleteApplication(app);
  }
  while (flags.restartGame) {
    pain::Application *app = pain::createApplication();
    flags = pain::Pain::runAndDeleteApplication(app);
  }
  return 0;
}
