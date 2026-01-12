#include <iostream>
#include <pain.h>

#include "Asteroid.h"
#include "Editor.h"
#include "MapGen/MainGen.h"
#include "Misc/CameraSys.h"
#include "MousePointer.h"
#include "Player.h"
#include "Spawner.h"
#include "Stars.h"
#include "Wall.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <vector>

class MainScript : public pain::WorldObject
{
public:
  static MainScript &createScriptScene(pain::Scene &scene, int cameraWidth,
                                       int cameraHeight, float zoom,
                                       pain::Application *app)
  {
    glm::vec2 playerPos{0.1, 0.1f};
    pain::Texture &shipTex =
        pain::resources::getTexture("resources/textures/ship_H.png");
    reg::Entity playerCam =
        Player::create(scene, shipTex, {0.2f, 0.2f}, playerPos, cameraWidth,
                       cameraHeight, zoom);
    app->setRendererCamera(playerCam);

    // add objects to collision System
    // scene.getSys<pain::Systems::SweepAndPruneSys>().insertColliders(walls);
    // ASETROID SPAWNER
    // reg::Entity spawner = Spawner::create(scene);
    // scene.emplaceLuaScript(spawner, "resources/scripts/spawner.lua");
    // MOUSE POINTER
    // ---------------------------------------------------------------
    // scene.emplaceScript<MousePointerScript>(mp, cameraEntity);

    // return scene.emplaceScript<MainScript>(scene.getEntity(),
    // std::move(stars),
    //                                        player, std::move(asteroids),
    //                                        std::move(walls), mp);
    // return scene.emplaceScript<MainScript>(scene.getEntity(), [&] {
    //   return MainMap::create(scene, 16.f, 16.f, playerPos, 32, 1);
    // });
    auto factory = [&] {
      // Force NRVO at last point
      return MainMap::create(scene, 16.f, 16.f, playerPos, 32, 1);
    };

    return pain::Scene::emplaceScript<MainScript>(scene.getEntity(), scene,
                                                  factory, playerCam);
  }
  void onUpdate(pain::DeltaTime dt)
  {
    m_mainMap.updateSurroundingChunks(
        getComponent<pain::Transform2dComponent>(m_player));
  }
  MainScript(reg::Entity entity, pain::Scene &scene, auto mainMapFactory,
             reg::Entity player)
      : pain::WorldObject(entity, scene), //
        m_player(player),                 //
        m_mainMap(mainMapFactory())       //
  {
    PLOG_I("Scene created {}", entity);
    PLOG_I("Player created {}", player);
  };
  std::vector<std::vector<int>> m_backgroundMap;
  reg::Entity m_player = reg::Entity{-1};
  MainMap m_mainMap;
};

pain::Application *pain::createApplication()
{
  if (!resources::existsFile(AppContext::configIniFile)) {
    PLOG_W("config.ini file should exist at the beginning! Did you skip the "
           "launcher?");
  }
  IniConfig ini;
  ini.readAndUpdate(AppContext::configIniFile);
  resources::setDefaultPath(ini.assetsPath.value.c_str());

  InternalConfig internalIni;
  internalIni.readAndUpdate(ini.assetsPath.value.c_str(),
                            INTERNAL_SETTINGS_STRING);

  LOG_T("Creating app");
  std::string title = internalIni.title.get();
  const int height = ini.defaultHeight.get();
  const int width = ini.defaultWidth.get();
  const float zoom = internalIni.zoomLevel.get();
  const float gridSize = internalIni.gridSize.get();

  Application *app = Application::createApplication(
      {.title = title.c_str(), .defaultWidth = width, .defaultHeight = height},
      {.swapChainTarget = false});

  pain::Scene &scene =
      app->createWorldSceneComponents(gridSize, pain::NativeScriptComponent{});

  scene.addSystem<Systems::SweepAndPruneSys>();
  scene.addSystem<Systems::Render>();
  scene.addSystem<Systems::NativeScript>();
  scene.addSystem<Systems::LuaScript>();
  scene.addSystem<Systems::Kinematics>();
  scene.addSystem<Systems::LuaSchedulerSys>();
  MainScript::createScriptScene(scene, width, height, zoom, app);

  UIScene &uiScene = app->createUIScene(pain::ImGuiComponent{});
  UIScene::emplaceImGuiScript<PainlessEditor>(uiScene.getEntity(), uiScene,
                                              *app);

  // Scene *scene = new MainScene(app->getLuaState());
  // ((MainScene *)scene)->init(*app, scene, (float)width / height, 1.0f);

  return app;
}

#ifdef PLATFORM_IS_LINUX
int main(int argc, char *argv[])
{
  bool isSettingsGuiNeeded = pain::Pain::initiate();
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
#elif defined PLATFORM_IS_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine,
                   int nCmdShow)
{
  bool isSettingsGuiNeeded = pain::Pain::initiate();
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
#endif
