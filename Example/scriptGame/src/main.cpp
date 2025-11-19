#include <iostream>
#include <pain.h>

#include "Assets/DefaultTexture.h"
#include "Asteroid.h"
#include "CoreFiles/LogWrapper.h"
#include "GUI/ImGuiComponent.h"
#include "Player.h"
#include "Stars.h"
#include "Wall.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class ImGuiQuickMenu : public pain::ExtendedEntity
{
public:
  void onRender(pain::Renderer2d &renderer, bool isMinimized,
                double currentTime)
  {
    ImGui::Begin("Debug info");
    auto &cam =
        m_scene.get().getComponent<pain::OrthoCameraComponent>(m_cameraEntity);

    ImGui::Text("Camera zoom: %.3f", cam.m_zoomLevel);
    ImGui::Text("Mouse Pos: (%d,%d)", TP_VEC2(m_mousePos));
    ImGui::Text("Mouse World Pos: (%.2f,%.2f)", TP_VEC2(m_mouseWorldPos));
    ImGui::Text("cellSize %.3f", m_app->getCellSize());
    float cellScreenPixels = (m_app->getCellSize() / cam.m_zoomLevel) *
                             cam.m_matrices->getResolution().y;
    ImGui::Text("cellScreenPixels %.3f", cellScreenPixels);

    ImGui::End();
  }
  void onEvent(const SDL_Event &event)
  {
    if (event.type == SDL_MOUSEMOTION) {
      int mouseX = event.motion.x;
      int mouseY = event.motion.y;
      m_mousePos = glm::ivec2(mouseX, mouseY);
      m_mouseWorldPos = screenToWorld(mouseX, mouseY);
    }
  }
  glm::vec2 screenToWorld(int mouseX, int mouseY)
  {
    auto &cc =
        m_scene.get().getComponent<pain::OrthoCameraComponent>(m_cameraEntity);
    auto &tc =
        m_scene.get().getComponent<pain::TransformComponent>(m_cameraEntity);
    auto &rc =
        m_scene.get().getComponent<pain::RotationComponent>(m_cameraEntity);

    // 1. screen → NDC
    float ndcX = (2.0f * mouseX) / cc.m_matrices->getResolution().x - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / cc.m_matrices->getResolution().y;

    // 2. NDC → camera local
    float camLocalX = ndcX * (cc.m_aspectRatio * cc.m_zoomLevel);
    float camLocalY = ndcY * cc.m_zoomLevel;

    // 3. camera rotation vectors
    float angle = rc.m_rotationAngle;
    glm::vec2 forward = glm::vec2(cos(angle), sin(angle));
    glm::vec2 right = glm::vec2(-sin(angle), cos(angle));

    // 4. camera local → world
    return glm::vec2(tc.m_position) + forward * camLocalY + right * camLocalX;
  }

  static void createScript(pain::Scene &scene, pain::Application *settingsApp,
                           reg::Entity cameraEntity)
  {
    scene.withImGuiScript<ImGuiQuickMenu>(
        ImGuiQuickMenu(scene.getEntity(), scene, settingsApp, cameraEntity));
  }

  ImGuiQuickMenu(reg::Entity entity, pain::Scene &scene, pain::Application *app,
                 reg::Entity cameraEntity)
      : ExtendedEntity(entity, scene), //
        m_cameraEntity(cameraEntity),  //
        m_app(app) {};

  ImGuiQuickMenu(ImGuiQuickMenu &&other) noexcept
      : ExtendedEntity(std::move(other)), m_cameraEntity(other.m_cameraEntity),
        m_app(std::exchange(other.m_app, nullptr)) {};
  ImGuiQuickMenu &operator=(ImGuiQuickMenu &&other) noexcept
  {
    if (this != &other) {
      ExtendedEntity::operator=(std::move(other));
      m_app = std::exchange(other.m_app, nullptr);
      m_cameraEntity = other.m_cameraEntity;
    }
    return *this;
  }

private:
  reg::Entity m_cameraEntity;
  glm::vec2 m_mouseWorldPos;
  glm::ivec2 m_mousePos;
  pain::Application *m_app = nullptr;
};

class MainScript : public pain::ExtendedEntity
{
public:
  static reg::Entity createScriptScene(pain::Scene &scene, int resWeight,
                                       int resHeight, float zoom,
                                       pain::Application *app)
  {

    // create the camera
    pain::OrthoCamera orthocamera = {&scene, resWeight, resHeight, zoom};
    orthocamera.withScript<pain::OrthoCameraScript>(scene);
    app->setRendererCamera(
        *(std::as_const(orthocamera)
              .getComponent<pain::OrthoCameraComponent>(scene)
              .m_matrices),
        orthocamera.getEntity());

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
    walls.reserve(4);
    pain::GridManager gm;
    // const glm::vec2 v1 = glm::vec2(-1.f, 1.f);
    // const glm::vec2 v2 = glm::vec2(1.f, 1.f);
    // const glm::vec2 v3 = glm::vec2(-1.f, -1.f);
    // const glm::vec2 v4 = glm::vec2(1.f, -1.f);
    walls.emplace_back(scene, gm, glm::vec2(-2.f, 2.f), glm::vec2(5.f, 1.f));
    walls.emplace_back(scene, gm, glm::vec2(2.f, 2.f), glm::vec2(1.f, 5.f));
    walls.emplace_back(scene, gm, glm::vec2(-2.f, -2.f), glm::vec2(1.f, 5.f));
    walls.emplace_back(scene, gm, glm::vec2(2.f, -2.f), glm::vec2(5.f, 1.f));
    // walls.emplace_back(std::move(w));
    // walls.emplace_back(std::move(w2));
    // walls.emplace_back(std::move(w3));
    // walls.emplace_back(std::move(w4));
    // scene.insertStaticCollider(ast.getEntity());

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
    for (short i = 0; i < asteroidAmount; i++) {
      glm::vec2 randomPos(dist(gen), dist(gen));
      glm::vec2 randomVel(dist(gen) * 0.1, dist(gen) * 0.1);
      Asteroid ast = {scene,                                        //
                      gm,                                           //
                      asteroidSheet,                                //
                      static_cast<short>(i % asteroidSheet.size()), //
                      randomPos,                                    //
                      randomVel};
      asteroids.emplace_back(std::move(ast));
      // scene.insertStaticCollider(ast.getEntity());
    }
    reg::Entity cameraEntity = orthocamera.getEntity();
    scene.withScript<MainScript>(std::move(stars), std::move(orthocamera),
                                 std::move(asteroids), std::move(walls),
                                 std::move(gm));

    return cameraEntity;
  }
  void onRender(pain::Renderer2d &renderer, bool minimazed, double deltatime)
  {
    renderer.drawQuad(
        {0.2f, -0.2f}, {0.3f, 0.4f}, {0.2f, 0.3f, 0.9f, 1.f},
        pain::resources::getDefaultTexture(pain::resources::BLANK, false));
  }

  MainScript(reg::Entity entity, pain::Scene &scene, std::vector<Stars> &&stars,
             pain::OrthoCamera &&orthocamera, std::vector<Asteroid> &&ast,
             std::vector<Wall> &&walls, pain::GridManager &&gm)
      : ExtendedEntity(entity, scene), m_orthocamera(std::move(orthocamera)),
        m_stars(std::move(stars)), m_asteroids(std::move(ast)),
        m_walls(std::move(walls)), m_gridManager(std::move(gm)) {};

  std::vector<std::vector<int>> m_backgroundMap;
  pain::OrthoCamera m_orthocamera;
  std::shared_ptr<pain::Shader> m_texture_shader;
  std::vector<Stars> m_stars;
  std::vector<Asteroid> m_asteroids;
  std::vector<Wall> m_walls;
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
  const int width = 1000;
  const int height = 1000;
  Application *app = Application::createApplication(title, width, height);

  pain::Scene &scene = app->createScene(1.f, pain::NativeScriptComponent{},
                                        pain::ImGuiComponent{});
  reg::Entity cameraEntity =
      MainScript::createScriptScene(scene, width, height, 2.0f, app);

  ImGuiQuickMenu::createScript(scene, app, cameraEntity);
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
