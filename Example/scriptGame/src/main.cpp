#include <iostream>
#include <pain.h>

#include "Asteroid.h"
#include "Editor.h"
#include "MousePointer.h"
#include "Player.h"
#include "Spawner.h"
#include "Stars.h"
#include "Wall.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class MainScript : public pain::ExtendedEntity
{
public:
  static MainScript &createScriptScene(pain::Scene &scene, int cameraWidth,
                                       int cameraHeight, float zoom,
                                       pain::Application *app)
  {
    // PLAYER ---------------------------------------------------------------
    // pain::Texture &shipTex =
    //     pain::resources::getTexture("resources/textures/ship_H.png");
    // reg::Entity player = Player::create(scene, shipTex, {0.2f, 0.2f},
    //                                     cameraWidth, cameraHeight, zoom);
    //
    //
    // // create the camera
    // Player::Script &playerScript =
    // scene.emplaceScript<Player::Script>(player);
    // scene.emplaceLuaScript(player, "resources/scripts/lua_script.lua");
    // app->setRendererCamera(playerScript.getEntity());
    // reg::Entity cameraEntity = playerScript.getEntity();

    // STARS ---------------------------------------------------------------
    // pain::TextureSheet &texSheet = pain::resources::createTextureSheet(
    //     "star_animation", // Unique name
    //     "resources/textures/star_animation_strip.png", 1, 4,
    //     {{0, 0}, {1, 0}, {2, 0}, {3, 0}});
    //
    // std::vector<reg::Entity> stars;
    // std::mt19937 gen(1337);
    // std::uniform_real_distribution<float> dist(-1.f, 1.f);
    // stars.reserve(starAmout);
    // for (unsigned short i = 0; i < starAmout; i++) {
    //   glm::vec2 randomPos(dist(gen), dist(gen));
    //   stars.emplace_back(Stars::create(
    //       scene, texSheet, static_cast<unsigned short>(i % texSheet.size()),
    //       randomPos));
    // }

    // WALLS ---------------------------------------------------------------
    std::vector<reg::Entity> walls;
    // walls.emplace_back(scene, glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 6.2f));
    walls.reserve(5);
    // walls.emplace_back(
    //     Wall::create(scene, glm::vec2(6.f, 4.f), glm::vec2(36.f, 1.f)));
    // walls.emplace_back(
    //     Wall::create(scene, glm::vec2(0.f, 2.f), glm::vec2(36.f, 1.f)));
    // walls.emplace_back(
    //     Wall::create(scene, glm::vec2(2.f, 0), glm::vec2(1.f, 36.f)));
    // walls.emplace_back(
    //     Wall::create(scene, glm::vec2(-2.f, 0), glm::vec2(1.f, 36.f)));
    // walls.emplace_back(
    //     Wall::create(scene, glm::vec2(0, -2.f), glm::vec2(36.f, 1.f)));
    // ASTEROID ---------------------------------------------------------------
    pain::TextureSheet &asteroidSheet = pain::resources::createTextureSheet(
        "asteroids", "resources/textures/asteroid.png", 1, 5,
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}});

    // std::vector<reg::Entity> asteroids;
    // asteroids.reserve(asteroidAmount);
    // for (short i = 0; i < asteroidAmount / 2; i++) {
    //   glm::vec2 randomPos(dist(gen), dist(gen));
    //   glm::vec2 randomVel(dist(gen), dist(gen));
    //   asteroids.emplace_back(Asteroid::create(          //
    //       scene,                                        //
    //       asteroidSheet,                                //
    //       static_cast<short>(i % asteroidSheet.size()), //
    //       std::move(randomPos),                         //
    //       std::move(randomVel), 0.1f));
    // }
    // for (short i = asteroidAmount / 2; i < asteroidAmount; i++) {
    //   glm::vec2 randomPos(dist(gen), dist(gen));
    //   glm::vec2 randomVel(dist(gen), dist(gen));
    //   asteroids.emplace_back(Asteroid::create(          //
    //       scene,                                        //
    //       asteroidSheet,                                //
    //       static_cast<short>(i % asteroidSheet.size()), //
    //       std::move(randomPos),                         //
    //       std::move(randomVel),                         //
    //       glm::vec2{0.2f, 0.2f}));                      //
    // }

    // for (unsigned short i = 0; i < asteroidAmount / 2; i++) {
    //   asteroids.emplace_back(Asteroid::create( //
    //       scene,                               //
    //       asteroidSheet,                       //
    //       i % asteroidSheet.size(),            //
    //       glm::vec2{-1.f + 0.3f * i, 0.2f},    //
    //       glm::vec2{0.5f, 1.0f},               //
    //       0.1f                                 //
    //       ));
    //   // glm::vec2{0.2f, 0.2f})); //
    // }
    // for (unsigned short i = asteroidAmount / 2; i < asteroidAmount; i++) {
    //   asteroids.emplace_back(Asteroid::create( //
    //       scene,                               //
    //       asteroidSheet,                       //
    //       i % asteroidSheet.size(),            //
    //       glm::vec2{0.3f * i, 0.2f},           //
    //       glm::vec2{0.5f, 1.0f},               //
    //       glm::vec2{0.2f, 0.2f}                //
    //       ));                                  //
    // }

    // add objects to collision System
    // scene.getSys<pain::Systems::SweepAndPruneSys>().insertColliders(walls);
    // ASETROID SPAWNER
    reg::Entity spawner = Spawner::create(scene);
    scene.emplaceLuaScript(spawner, "resources/scripts/spawner.lua");
    // MOUSE POINTER
    // ---------------------------------------------------------------
    // scene.emplaceScript<MousePointerScript>(mp, cameraEntity);

    // return scene.emplaceScript<MainScript>(scene.getEntity(),
    // std::move(stars),
    //                                        player, std::move(asteroids),
    //                                        std::move(walls), mp);
    return scene.emplaceScript<MainScript>(scene.getEntity());
  }
  void onRender(pain::Renderer2d &renderer, bool minimazed,
                pain::DeltaTime deltatime)
  {
    // renderer.drawQuad(
    //     {0.2, -0.2f}, {0.2f, 0.2f}, {0.8f, 0.2f, 0.1f, 1.f},
    //     0.25f * std::numbers::pi,
    //     pain::resources::getTexture("resources/textures/Checkerboard.png"));
    // renderer.drawQuad(
    //     {0.2f, 0.2f}, {0.2f, 0.2f}, {0.9f, 0.9f, 0.2f, 1.f},
    //     pain::resources::getDefaultTexture(pain::resources::Blank, false));
    // renderer.drawQuad({-0.2f, 0.2f}, {0.2f, 0.2f}, {1.f, 1.f, 1.f, 1.f},
    //                   pain::resources::getTexture(
    //                       "resources/textures/Checkerboard original.png"));
    // renderer.drawCircle({0.0f, 0.0f}, 0.1f, {0.2f, 0.3f, 0.9f, 1.f});
    // renderer.drawTri({-0.2f, -0.2f}, {0.2f, 0.2f}, {0.2f, 0.3f, 0.9f, 1.f});
  }
  MainScript(reg::Entity entity, pain::Scene &scene)
      : ExtendedEntity(entity, scene) {};

  MainScript(reg::Entity entity, pain::Scene &scene,
             std::vector<reg::Entity> &&stars, reg::Entity orthocamera,
             std::vector<reg::Entity> &&asteroid,
             std::vector<reg::Entity> &&walls, reg::Entity mp)
      : ExtendedEntity(entity, scene), m_orthocamera(orthocamera),
        m_stars(std::move(stars)), m_asteroids(std::move(asteroid)),
        m_walls(std::move(walls)), m_mousePointer(mp) {};

  std::vector<std::vector<int>> m_backgroundMap;
  reg::Entity m_orthocamera;
  std::shared_ptr<pain::Shader> m_textureShader;
  std::vector<reg::Entity> m_stars;
  std::vector<reg::Entity> m_asteroids;
  std::vector<reg::Entity> m_walls;
  reg::Entity m_mousePointer;
  // Player m_player;
  const static unsigned starAmout = 0;
  const static unsigned asteroidAmount = 20;
};

pain::Application *pain::createApplication()
{
  P_ASSERT(resources::exists_file(Application::configIniFile),
           "config.ini file must exist at the beginning");
  IniConfig ini;
  ini.readAndUpdate(Application::configIniFile);
  resources::setDefaultPath(ini.assetsPath.value.c_str());

  LOG_T("Creating app");
  const char *title = "Developing Pain - Example 2d";
  const int width = 1000;
  const int height = 1000;
  const float zoom = 5.f;

  Application *app = Application::createApplication(title, width, height,
                                                    {.swapChainTarget = false});

  pain::Scene &scene =
      app->createWorldSceneComponents(1.f, pain::NativeScriptComponent{});

  scene.addSystem<Systems::SweepAndPruneSys>();
  scene.addSystem<Systems::Render>();
  scene.addSystem<Systems::NativeScript>();
  scene.addSystem<Systems::LuaScript>();
  scene.addSystem<Systems::Kinematics>();
  scene.addSystem<Systems::LuaSchedulerSys>();
  MainScript::createScriptScene(scene, width, height, zoom, app);

  UIScene &uiScene = app->createUIScene(pain::ImGuiComponent{});
  uiScene.emplaceImGuiScript<PainlessEditor>(uiScene.getEntity(), *app);

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
