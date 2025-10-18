#include <iostream>
#include <pain.h>

#include "Asteroid.h"
#include "Player.h"
#include "Stars.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class MainScript : public pain::ExtendedEntity
{
public:
  static void createScript(pain::Scene &scene, float aspectRatio, float zoom,
                           pain::Application *app)
  {
    // create the camera
    std::unique_ptr<pain::OrthoCamera> orthocamera =
        std::make_unique<pain::OrthoCamera>(&scene, aspectRatio, zoom);
    orthocamera->withScript<pain::OrthoCameraScript>(scene);
    app->setRendererCamera(
        *(std::as_const(orthocamera)
              ->getComponent<pain::OrthoCameraComponent>(scene)
              .m_camera));

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
    pain::GridManager gm;
    for (short i = 0; i < asteroidAmount; i++) {
      glm::vec2 randomPos(dist(gen), dist(gen));
      glm::vec2 randomVel(dist(gen), dist(gen));
      Asteroid ast = {scene,                                        //
                      gm,                                           //
                      asteroidSheet,                                //
                      static_cast<short>(i % asteroidSheet.size()), //
                      randomPos,                                    //
                      randomVel};
      asteroids.emplace_back(std::move(ast));
      scene.insertStaticCollider(ast.getEntity(), ast.getBitMask());
    }
    scene.withScript<MainScript>(aspectRatio, zoom, app, std::move(stars),
                                 std::move(orthocamera), std::move(asteroids));
  }

  MainScript(pain::Entity entity, pain::Bitmask bitmask, pain::Scene &scene,
             float aspectRatio, float zoom, pain::Application *app,
             std::vector<Stars> &&stars,
             std::unique_ptr<pain::OrthoCamera> orthocamera,
             std::vector<Asteroid> &&ast, Player &&player)
      : ExtendedEntity(entity, bitmask, scene),
        m_orthocamera(std::move(orthocamera)), m_stars(std::move(stars)),
        m_asteroids(std::move(ast)) {};

  std::vector<std::vector<int>> m_backgroundMap;
  std::unique_ptr<pain::OrthoCamera> m_orthocamera;
  std::shared_ptr<pain::Shader> m_texture_shader;
  std::vector<Stars> m_stars;
  std::vector<Asteroid> m_asteroids;
  // Player m_player;
  pain::GridManager m_gridManager;
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
  const int width = 1280;
  const int height = 1000;
  Application *app = Application::createApplication(title, width, height);

  Scene *scene = app->createScenePtr<Scene>("main", NativeScriptComponent{});
  MainScript::createScript(*scene, (float)width / height, 1.0f, app);
  // Scene *scene = new MainScene(app->getLuaState());
  // ((MainScene *)scene)->init(*app, scene, (float)width / height, 1.0f);

  app->pushScene("main", scene);
  app->attachScene("main");
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
