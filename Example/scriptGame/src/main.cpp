/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <pain.h>

#include "Assets/ManagerTexture.h"
#include "Asteroid.h"
#include "Dumb.h"
#include "Editor.h"
#include "MapGen/MainGen.h"
#include "MousePointer.h"
#include "Player.h"
#include "Player3d.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <vector>

class MainScript : public pain::WorldObject
{
public:
  static MainScript &createScriptScene(pain::Scene &scene, int cameraWidth,
                                       int cameraHeight, pain::Application *app)
  {
    glm::vec2 playerPos{0};
    glm::vec2 playerSize{0.25f};
    pain::TextureManager::createTexture(
        "resources/default/textures/Checkerboard arrow.png");
    pain::Texture &playerTex = pain::TextureManager::getDefaultTexture(
        pain::TextureManager::DefaultTexture::General, false);
    reg::Entity playerCam =
        Player::create(scene, playerTex, playerPos, playerSize, cameraWidth,
                       cameraHeight, 5.f);
    reg::Entity lightSource = DumbObject::create(scene);
    // app->set2dRendererCamera(playerCam, cameraWidth, cameraHeight);
    app->set2dRendererCamera(playerCam, cameraWidth, cameraHeight);
    // app->getRenderers().renderer3d.changeLight(lightSource);
    // add objects to collision System
    // scene.getSys<pain::Systems::SweepAndPruneSys>().insertColliders(walls);
    // ASETROID SPAWNER
    // reg::Entity spawner = Spawner::create(scene);
    // scene.emplaceLuaScript(spawner, "resources/scripts/spawner.lua");
    // MOUSE POINTER
    // ---------------------------------------------------------------
    // scene.emplaceScript<MousePointerScript>(mp, cameraEntity);
    //
    return pain::Scene::emplaceScript<MainScript>(scene.getEntity(), scene,
                                                  playerCam, lightSource);
  }
  // void onCreate() { m_mainMap.onCreate(getScene()); }
  MainScript(reg::Entity entity, pain::Scene &scene, reg::Entity player,
             reg::Entity lightsource)
      : pain::WorldObject(entity, scene), //
        m_player(player),                 //
        m_light(lightsource)              //
  // m_mainMap(mainMapFactory())       //
  {};
  std::vector<std::vector<int>> m_backgroundMap;
  reg::Entity m_player = reg::Entity{-1};
  reg::Entity m_light = reg::Entity{-1};
  // MainMap m_mainMap;

  // void onUpdate(pain::DeltaTime dt)
  // {
  //   m_mainMap.updateSurroundingChunks(
  //       getComponent<pain::Transform2dComponent>(m_player), getScene());
  // }
  void onRender(pain::Renderers &renderer, bool isMinimazed,
                pain::DeltaTime currentTime)
  {
    pain::Texture &blank = pain::TextureManager::getDefaultTexture(
        pain::TextureManager::DefaultTexture::Blank, false);
    pain::Texture &simple = pain::TextureManager::getDefaultTexture(
        pain::TextureManager::DefaultTexture::General, false);
    // renderer.renderer3d.drawCube({0.f, 0.f, 0.f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::Purple, simple);
    // renderer.renderer3d.drawCube({0.5f, 0.5f, 0.5f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::Blue, simple);
    // renderer.renderer3d.drawCube({0.5f, 0.5f, -0.5f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::Grey, simple);
    // renderer.renderer3d.drawCube({0.5f, -0.5f, 0.5f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::Lime, simple);
    // renderer.renderer3d.drawCube({-0.5f, 0.5f, 0.5f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::Navy, simple);
    // renderer.renderer3d.drawCube({0.5f, -0.5f, -0.5f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::Red, simple);
    // renderer.renderer3d.drawCube({-0.5f, 0.5f, -0.5f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::SoftPink, simple);
    // renderer.renderer3d.drawCube({-0.5f, -0.5f, 0.5f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::OffWhite, simple);
    // renderer.renderer3d.drawCube({-0.5f, -0.5f, -0.5f}, {0.3f, 0.3f, 0.3f},
    //                              pain::Colors::Olive, simple);

    // renderer.renderer3d.drawUVSphere({0.f, 0.f, 0.f}, {0.3f, 0.3f, 0.3f},
    //                                  pain::Colors::Red, simple);
    // renderer.renderer3d.drawUVSphere({0.5f, 0.5f, 0.5f}, {0.3f, 0.3f, 0.3f},
    //                                  pain::Colors::Blue, simple,
    //                                  pain::SphereDivision::D_32x32);
    // renderer.renderer3d.drawUVSphere({0.5f, 0.5f, -0.5f}, {0.3f, 0.3f, 0.3f},
    //                                  pain::Colors::Green, simple,
    //                                  pain::SphereDivision::D_32x32);
    // renderer.renderer3d.drawUVSphere({0.5f, -0.5f, 0.5f}, {0.3f, 0.3f, 0.3f},
    //                                  pain::Colors::Yellow, simple,
    //                                  pain::SphereDivision::D_32x32);
    // renderer.renderer3d.drawUVSphere({-0.5f, 0.5f, 0.5f}, {0.3f, 0.3f, 0.3f},
    //                                  pain::Colors::Cyan, simple,
    //                                  pain::SphereDivision::D_32x32);
    // renderer.renderer3d.drawUVSphere({0.5f, -0.5f, -0.5f}, {0.3f, 0.3f,
    // 0.3f},
    //                                  pain::Colors::Purple, simple,
    //                                  pain::SphereDivision::D_32x32);
    // renderer.renderer3d.drawUVSphere({-0.5f, 0.5f, -0.5f}, {0.3f, 0.3f,
    // 0.3f},
    //                                  pain::Colors::OffWhite, simple,
    //                                  pain::SphereDivision::D_32x32);
    // renderer.renderer3d.drawUVSphere({-0.5f, -0.5f, 0.5f}, {0.3f, 0.3f,
    // 0.3f},
    //                                  pain::Colors::DarkerGrey, simple,
    //                                  pain::SphereDivision::D_32x32);
    // renderer.renderer3d.drawUVSphere({-0.5f, -0.5f, -0.5f}, {0.3f, 0.3f,
    // 0.3f},
    //                                  pain::Colors::Brown, simple,
    //                                  pain::SphereDivision::D_32x32);

    renderer.renderer2d.drawQuad({0.0f, 0.0f}, {0.25f, 0.25f},
                                 {230, 230, 51, 255},
                                 std::numbers::pi * -currentTime.getSeconds(),
                                 pain::RenderLayer::Default, simple);
    renderer.renderer2d.drawQuad({0.0f, 0.0f}, {0.25f, 0.25f},
                                 {230, 230, 51, 255},
                                 pain::RenderLayer::Default, simple);
    renderer.renderer2d.drawQuad({-0.5f, 0.5f}, {0.25f, 0.25f},
                                 {255, 255, 255, 255},
                                 pain::RenderLayer::Default, simple);
    renderer.renderer2d.drawQuad({0.2f, -0.2f}, {0.25f, 0.25f},
                                 {204, 51, 26, 128},
                                 std::numbers::pi * currentTime.getSeconds(),
                                 pain::RenderLayer::MuchCloser, simple);
    renderer.renderer2d.drawQuad({0.2f, 0.2f}, {0.25f, 0.25f},
                                 {230, 230, 51, 255},
                                 pain::RenderLayer::MuchCloser, simple);
    renderer.renderer2d.drawQuad({-0.2f, 0.2f}, {0.25f, 0.25f},
                                 {255, 255, 255, 255},
                                 pain::RenderLayer::MuchCloser, simple);
    renderer.renderer2d.drawCircle({0.5f, 0.5f}, 0.25f, {51, 75, 230, 255});
    renderer.renderer2d.drawTri({-0.5f, -0.5f}, {0.25f, 0.25f},
                                {0.2f, 0.3f, 0.9f, 1.f});
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
       .defaultHeight = ini.defaultHeight.get(),             //
       .is3d = internalIni.is3d.get()},                      //
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
