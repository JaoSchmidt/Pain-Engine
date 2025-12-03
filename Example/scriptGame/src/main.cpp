#include <iostream>
#include <pain.h>

#include "Assets/DefaultTexture.h"
#include "Asteroid.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/Movement.h"
#include "GUI/ImGuiComponent.h"
#include "GUI/ImGuiDebugMenu.h"
#include "MousePointer.h"
#include "Player.h"
#include "Stars.h"
#include "Wall.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class MainScript : public pain::ExtendedEntity
{
public:
  static MainScript &createScriptScene(pain::Scene &scene, int resWeight,
                                       int resHeight, float zoom,
                                       pain::Application *app)
  {

    // create the camera
    pain::OrthoCamera orthocamera = {&scene, resWeight, resHeight, zoom};
    orthocamera.emplaceScript<pain::OrthoCameraScript>(scene);
    app->setRendererCamera(
        *(std::as_const(orthocamera)
              .getComponent<pain::OrthoCameraComponent>(scene)
              .m_matrices),
        orthocamera.getEntity());
    reg::Entity cameraEntity = orthocamera.getEntity();

    // dummy.reset(new Dummy(&scene, {0.23f, 0.54f}, {1.f, 1.f},
    //                       {9.f, 0.f, 5.f, 1.f}, &m_texture, 1.f));
    // auto ls = &dummy->getComponent<pain::LuaScriptComponent>(scene);
    // ls->bind("resources/scripts/lua_script.lua");
    // ls->onCreate();

    // STARS ---------------------------------------------------------------
    pain::TextureSheet &texSheet = pain::resources::createTextureSheet(
        "star_animation", // Unique name
        "resources/textures/star_animation_strip.png", 1, 4,
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}});

    std::vector<Stars> stars;
    std::mt19937 gen(1337);
    std::uniform_real_distribution<float> dist(-1.f, 1.f);
    stars.reserve(starAmout);
    for (short i = 0; i < starAmout; i++) {
      glm::vec2 randomPos(dist(gen), dist(gen));
      Stars s = {scene, texSheet, static_cast<short>(i % texSheet.size()),
                 randomPos};
      stars.emplace_back(std::move(s));
    }

    // WALLS ---------------------------------------------------------------
    std::vector<Wall> walls;
    // walls.reserve(1);
    // walls.emplace_back(scene, glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 6.2f));
    walls.reserve(4);
    walls.emplace_back(scene, glm::vec2(-2.f, 2.f), glm::vec2(8.f, 1.f));
    walls.emplace_back(scene, glm::vec2(2.f, 2.f), glm::vec2(1.f, 8.f));
    walls.emplace_back(scene, glm::vec2(-2.f, -2.f), glm::vec2(1.f, 8.f));
    walls.emplace_back(scene, glm::vec2(2.f, -2.f), glm::vec2(8.f, 1.f));

    // PLAYER ---------------------------------------------------------------
    // pain::Texture &shipTex =
    //     pain::resources::getTexture("resources/textures/ship_H.png");
    // Player player = {scene, shipTex};
    // auto lsc = &player.getComponent<pain::LuaScriptComponent>(scene);
    // lsc->bind("resources/scripts/lua_script.lua");
    // ASTEROID ---------------------------------------------------------------
    pain::TextureSheet &asteroidSheet = pain::resources::createTextureSheet(
        "asteroids", "resources/textures/asteroid.png", 1, 5,
        {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}});
    std::vector<Asteroid> asteroids;
    asteroids.reserve(asteroidAmount);
    for (short i = 0; i < asteroidAmount / 2; i++) {
      glm::vec2 randomPos(dist(gen), dist(gen));
      glm::vec2 randomVel(dist(gen) * 0.5, dist(gen) * 0.5);
      asteroids.emplace_back(                           //
          scene,                                        //
          asteroidSheet,                                //
          static_cast<short>(i % asteroidSheet.size()), //
          std::move(randomPos),                         //
          std::move(randomVel), 0.1f);
    }
    for (short i = asteroidAmount / 2; i < asteroidAmount; i++) {
      glm::vec2 randomPos(dist(gen), dist(gen));
      glm::vec2 randomVel(dist(gen) * 0.5, dist(gen) * 0.5);
      asteroids.emplace_back(                           //
          scene,                                        //
          asteroidSheet,                                //
          static_cast<short>(i % asteroidSheet.size()), //
          std::move(randomPos),                         //
          std::move(randomVel),                         //
          glm::vec2{0.2f, 0.2f});                       //
    }

    //
    // asteroids.emplace_back(                           //
    //     scene,                                        //
    //     asteroidSheet,                                //
    //     static_cast<short>(0 % asteroidSheet.size()), //
    //     glm::vec2(-0.21f, -0.8f),                     //
    //     glm::vec2(0.01f, 0.1f),                       //
    //     0.1f                                          //
    // );
    // asteroids.emplace_back(                           //
    //     scene,                                        //
    //     asteroidSheet,                                //
    //     static_cast<short>(0 % asteroidSheet.size()), //
    //     glm::vec2{0.5f, 0.0f},                        //
    //     glm::vec2{-0.2f, 0.f},                        //
    //     0.1f                                          //
    // );
    // asteroids.emplace_back(                           //
    //     scene,                                        //
    //     asteroidSheet,                                //
    //     static_cast<short>(0 % asteroidSheet.size()), //
    //     glm::vec2(-0.21f, 0.8f),                      //
    //     glm::vec2(0.01f, -0.1f),                      //
    //     glm::vec2{0.2f, 0.2f}                         //
    // );
    //
    // MOUSE POINTER
    // ---------------------------------------------------------------
    MousePointer mp(scene);
    mp.emplaceScript<MousePointerScript>(scene, cameraEntity);

    scene.emplaceImGuiScript<pain::ImGuiDebugMenu>(app, cameraEntity,
                                                   mp.getEntity());
    return scene.emplaceScript<MainScript>(
        std::move(stars), std::move(orthocamera), std::move(asteroids),
        std::move(walls), std::move(mp));
  }
  void onRender(pain::Renderer2d &renderer, bool minimazed, double deltatime)
  {
    // renderer.drawQuad(
    //     {0.0f, 0.0f}, {0.2f, 0.2f}, {0.2f, 0.9f, 0.6f, 1.f},
    //     pain::resources::getDefaultTexture(pain::resources::BLANK, false));
    renderer.drawCircle({0.0f, 0.0f}, 0.2f, {0.2f, 0.3f, 0.9f, 1.f});
  }

  MainScript(reg::Entity entity, pain::Scene &scene, std::vector<Stars> &&stars,
             pain::OrthoCamera &&orthocamera, std::vector<Asteroid> &&ast,
             std::vector<Wall> &&walls, MousePointer &&mp)
      : ExtendedEntity(entity, scene), m_orthocamera(std::move(orthocamera)),
        m_stars(std::move(stars)), m_asteroids(std::move(ast)),
        m_walls(std::move(walls)), m_mousePointer(std::move(mp))
  {
    m_orthocamera.getEntity();
  };

  std::vector<std::vector<int>> m_backgroundMap;
  pain::OrthoCamera m_orthocamera;
  std::shared_ptr<pain::Shader> m_texture_shader;
  std::vector<Stars> m_stars;
  std::vector<Asteroid> m_asteroids;
  std::vector<Wall> m_walls;
  MousePointer m_mousePointer;
  // Player m_player;
  const static unsigned starAmout = 12;
  const static unsigned asteroidAmount = 12;
};

pain::Application *pain::createApplication()
{
  P_ASSERT(resources::exists_file(Application::configIniFile),
           "config.ini file must exist at the beginning");
  IniConfig ini;
  ini.readAndUpdate(Application::configIniFile.c_str());
  resources::setDefaultPath(ini.assetsPath.value.c_str());

  LOG_T("Creating app");
  const char *title = "Developing Pain - Example 2d";
  const int width = 1000;
  const int height = 1000;
  const float zoom = 1.f;
  Application *app = Application::createApplication(title, width, height);

  pain::Scene &scene = app->createScene(1.f, pain::NativeScriptComponent{},
                                        pain::ImGuiComponent{});
  MainScript::createScriptScene(scene, width, height, zoom, app);

  // Scene *scene = new MainScene(app->getLuaState());
  // ((MainScene *)scene)->init(*app, scene, (float)width / height, 1.0f);

  return app;
}

#ifdef _WIN64
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine,
                   INT nCmdShow)
{
  pain::Pain::initiate();
  pain::Pain::runApplication(pain::CreateApplication());
  return 0;
}
#else
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
#endif
