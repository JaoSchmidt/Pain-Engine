#include <iostream>
#include <pain.h>

#include "ECS/Components/NativeScript.h"
#include "dummy.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class MainScript : public pain::ExtendedEntity
{
public:
  MainScript(pain::Entity entity, pain::Bitmask bitmask, pain::Scene &scene,
             float aspectRatio, float zoom, pain::Application *app)
      : ExtendedEntity(entity, bitmask, scene),
        m_texture(pain::resources::getTexture(
            "resources/textures/Checkerboard.png", true))
  {
    // create the camera
    m_orthocamera =
        std::make_unique<pain::OrthoCamera>(&scene, aspectRatio, zoom);
    m_orthocamera->withScript<pain::OrthoCameraScript>(scene);
    app->setRendererCamera(
        *(std::as_const(m_orthocamera)
              ->getComponent<pain::OrthoCameraComponent>(scene)
              .m_camera));

    // make the renderer main camera (not related to the ECS)
    // pain::Renderer2d::init(*m_orthocamera);

    // create the script component meaning it can now  extend the object

    // manually instantiate the controller and link it with the bitmask and
    // entity
    // scene->initializeScript(a, m_orthocamera->getEntity(),
    //                         m_orthocamera->getBitMask());
    // ShapesController *sc = new ShapesController();
    // app.addImGuiInstance(sc);
    // m_sc = sc;

    // m_mainMap = std::make_unique<MainMap>(16.0f, 16.0f, tc.m_position, this);
    // dummy = new Dummy(scene, {0.f, 0.f}, {1.f, 1.f}, {9.f, 0.f, 5.f, 1.f},
    //                   m_texture, 1.f);
    // ls = &dummy->getComponent<pain::LuaScriptComponent>(*scene);
    // ls->bind("resources/scripts/lua_script.lua");
    // ls->onCreate();
  };
  void onUpdate(double deltaTime)
  {
    // PROFILE_FUNCTION()
    // pain::TransformComponent &tc =
    //     m_orthocamera->getComponent<pain::TransformComponent>();
    // m_mainMap->updateSurroundingChunks(tc.m_position, this);
  }
  void onRender(pain::Renderer2d &renderer, bool isMinimized,
                double currentTime)
  {
    // m_orthocamera->onUpdate(deltaTime);
    // const std::vector<std::vector<int>> mdm =
    //     m_mainMap.generateTerrainMatrix(); // lowest (background) layer
    // const std::vector<std::vector<int>> msm =
    //     generateTerrainMatrix(offset); // upper layer

    // for (unsigned int i = 0; i < mdm[0].size(); i++) {
    //   for (unsigned int j = 0; j < mdm.size(); j++) {
    //     if (mdm[j][i] != 00)
    //       pain::Renderer2d::DECO*27 - ラビットホールdrawQuad(
    //           {1.f * i, -1.f * j}, {1.f, 1.f}, {1.0f, 1.0f, 1.0f, 1.0f},
    //           m_mainMap.getTexture(), 1.0f,
    //           m_mainMap.getTexCoord(mdm[j][i]));
    //   }
    // }
    // pain::Renderer2d::drawQuad({0.0f, -0.8f}, {0.3f, 0.3f},
    //                            {0.9f, 0.3f, 0.2f, 1.0f});
    // pain::Renderer2d::drawQuad({-0.5f, 0.0f}, {0.3f, 0.3f},
    //                            {0.8f, 0.9f, 0.3f, 1.0f});
    PLOG_I("banana");
    renderer.drawQuad({0.0f, 0.0f}, {0.4f, 0.4f}, {1.0f, 1.0f, 1.0f, 1.0f},
                      m_texture);
    // pain::Renderer2d::drawQuad({-0.5f, -0.5f}, {0.4f, 0.4f}, m_texture, 1.0f,
    //                            {1.0f, 1.0f, 1.0f, 1.0f});
  }
  void onEvent(const SDL_Event &event) {}

private:
  std::vector<std::vector<int>> m_backgroundMap;
  std::unique_ptr<pain::OrthoCamera> m_orthocamera;
  std::shared_ptr<pain::Shader> m_texture_shader;
  pain::Texture &m_texture;
  std::unique_ptr<Dummy> dummy;
  std::unique_ptr<pain::LuaScriptComponent> ls;
};

pain::Application *pain::createApplication()
{
  LOG_T("Creating app");
  const char *title = "Developing Pain - Example 2d";
  const int width = 1280;
  const int height = 1000;
  Application *app = Application::createApplication(title, width, height);

  std::unique_ptr<Scene> scene =
      app->createSceneUPtr<Scene>(NativeScriptComponent{});
  scene->withScript<MainScript>((float)width / height, 1.0f, app);
  // Scene *scene = new MainScene(app->getLuaState());
  // ((MainScene *)scene)->init(*app, scene, (float)width / height, 1.0f);

  app->pushScene("main", std::move(scene));
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
