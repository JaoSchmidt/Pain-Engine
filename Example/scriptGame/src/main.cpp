/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <pain.h>

#include "Asteroid.h"
#include "Editor.h"
#include "MapGen/MainGen.h"
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
    glm::vec2 playerPos{0};
    pain::Texture &shipTex =
        pain::TextureManager::createTexture("resources/textures/ship_H.png");
    reg::Entity playerCam =
        Player::create(scene, shipTex, playerPos, {0.25f, 0.25f}, cameraWidth,
                       cameraHeight, zoom);
    app->setRendererCamera(playerCam, cameraWidth, cameraHeight);
    scene.getComponent<Component::OrthoCamera>(playerCam).setProjection(
        cameraWidth, cameraHeight);

    // add objects to collision System
    // scene.getSys<pain::Systems::SweepAndPruneSys>().insertColliders(walls);
    // ASETROID SPAWNER
    // reg::Entity spawner = Spawner::create(scene);
    // scene.emplaceLuaScript(spawner, "resources/scripts/spawner.lua");
    // MOUSE POINTER
    // ---------------------------------------------------------------
    // scene.emplaceScript<MousePointerScript>(mp, cameraEntity);
    auto factory = [&] {
      // Force NRVO at last point
      return MainMap::create(16.f, 16.f, playerPos, 32, 1, 4.f);
    };

    return pain::Scene::emplaceScript<MainScript>(scene.getEntity(), scene,
                                                  factory, playerCam);
  }
  // void onCreate() { m_mainMap.onCreate(getScene()); }
  MainScript(reg::Entity entity, pain::Scene &scene, auto mainMapFactory,
             reg::Entity player)
      : pain::WorldObject(entity, scene), //
        m_player(player)                  //
  // m_mainMap(mainMapFactory())       //
  {};
  std::vector<std::vector<int>> m_backgroundMap;
  reg::Entity m_player = reg::Entity{-1};
  // MainMap m_mainMap;

  // void onUpdate(pain::DeltaTime dt)
  // {
  //   m_mainMap.updateSurroundingChunks(
  //       getComponent<pain::Transform2dComponent>(m_player), getScene());
  // }
  void onRender(pain::Renderer2d &renderer, bool isMinimazed,
                pain::DeltaTime currentTime)
  {
    // renderer.drawQuad({-0.2f, -0.2f}, {0.25f, 0.25f}, {204, 51, 25, 255},
    //                   std::numbers::pi * -currentTime.getSeconds(),
    //                   pain::RenderLayer::Default,
    //                   pain::TextureManager::getTexture(
    //                       "resources/textures/Checkerboard.png"));
    // renderer.drawQuad({0.0f, 0.0f}, {0.25f, 0.25f}, {230, 230, 51, 255},
    //                   pain::RenderLayer::Default,
    //                   pain::TextureManager::getDefaultTexture(
    //                       pain::TextureManager::General, false));
    //
    // renderer.drawQuad({-0.5f, 0.5f}, {0.25f, 0.25f}, {255, 255, 255, 255},
    //                   pain::RenderLayer::Default,
    //                   pain::TextureManager::getTexture(
    //                       "resources/textures/Checkerboard original.png"));
    // renderer.drawQuad({0.2f, -0.2f}, {0.25f, 0.25f}, {204, 51, 26, 255},
    //                   std::numbers::pi * currentTime.getSeconds(),
    //                   pain::RenderLayer::MuchCloser,
    //                   pain::TextureManager::getTexture(
    //                       "resources/textures/Checkerboard.png"));
    // renderer.drawQuad({0.2f, 0.2f}, {0.25f, 0.25f}, {230, 230, 51, 255},
    //                   pain::RenderLayer::MuchCloser,
    //                   pain::TextureManager::getDefaultTexture(
    //                       pain::TextureManager::General, false));
    // renderer.drawQuad({-0.2f, 0.2f}, {0.25f, 0.25f}, {255, 255, 255, 255},
    //                   pain::RenderLayer::MuchCloser,
    //                   pain::TextureManager::getTexture(
    //                       "resources/textures/Checkerboard original.png"));
    // renderer.drawCircle({0.5f, 0.5f}, 0.25f, {51, 75, 230, 255});
    // renderer.drawTri({-0.5f, -0.5f}, {0.25f, 0.25f}, {0.2f, 0.3f,
    // 0.9f, 1.f});
  }
};

pain::Application *pain::createApplication()
{
  // Retrieve the context the player will alter when using the launcher
  IniConfig ini;
  ini.readAndUpdate();

  // Retrieve the app context defined inside "resources/InternalConfig.ini"
  InternalConfig internalIni;
  internalIni.readAndUpdate(ini.assetsPath.value.c_str());

  // Create the application + OpenGL + Event contexts
  Application *app = Application::createApplication(         //
      {.title = internalIni.title.get().c_str(),             //
       .defaultWidth = ini.defaultWidth.get(),               //
       .defaultHeight = ini.defaultHeight.get()},            //
      {.swapChainTarget = internalIni.swapChainTarget.get()} //
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
  scene.addSystem<Systems::ParticleSys>();

  // (Optional) Defining a small native script (MainScript) for the world scene
  // that will be executed on. Must have added System::NativeScript
  MainScript::createScriptScene(     //
      scene, ini.defaultWidth.get(), //
      ini.defaultHeight.get(),       //
      internalIni.zoomLevel.get(),   //
      app                            //
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
