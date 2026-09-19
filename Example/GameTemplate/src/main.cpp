#include <pain.h>
#include <painless.h>

#include "MainScript.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

pain::Application *pain::createApplication()
{
  // Retrieve the context the player will alter when using the launcher
  IniConfig ini;
  ini.readAndUpdate();

  // Retrieve the app context defined inside "resources/InternalConfig.ini"
  InternalConfig internalIni;
  internalIni.readAndUpdate(ini.assetsPath.value);

  // Create the application + OpenGL + Event contexts
  Application *app = Application::createApplication(         //
      {.title = internalIni.title.get().c_str(),             //
       .defaultWidth = ini.defaultWidth.get(),               //
       .defaultHeight = ini.defaultHeight.get()},            //
      {.swapChainTarget = internalIni.swapChainTarget.get()} //
  );

  // Create the ECS World Scene
  pain::Scene &scene = app->getWorldScene();
  scene.createComponents(scene.getEntity(), cmp::Script{});

  // Individually add each system
  scene.addSystem<Systems::SweepAndPruneSys>();
  scene.addSystem<Systems::Render2d>();
  scene.addSystem<Systems::NativeScript>();
  scene.addSystem<Systems::LuaScript>();
  scene.addSystem<Systems::Kinematics>();
  scene.addSystem<Systems::LuaSchedulerSys>();

  // (Optional) Defining a small native script (MainScript) for the world scene
  // that will be executed on. Must have added System::NativeScript
  MainScript::createScriptScene( //
      scene,                     //
      app                        //
  );

  // (Optional) Creating the ECS UI scene
  UIScene &uiScene = app->createUIScene();
  //
  // (Optional) A small native script that works as our game engine editor
  painless::Editor &editor = painless::Editor::create(uiScene, *app);

  // (Optional) Define a small native script for the world scene
  // that will be executed on as root script. Must have added
  // System::NativeScript
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
    pain::Application *app = painless::createLauncher();
    flags = pain::Pain::runAndDeleteApplication(app);
  }
  while (flags.restartGame) {
    pain::Application *app = pain::createApplication();
    flags = pain::Pain::runAndDeleteApplication(app);
  }
  return 0;
}
