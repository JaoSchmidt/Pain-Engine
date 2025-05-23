#include <iostream>
#include <pain.h>

#include "dummy.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

class ImGuiController : public pain::ImGuiInstance
{
public:
  void onImGuiUpdate() override
  {
    ImGui::Begin("Shapes Controller");
    ImGui::End();
  }
};

class MainScene : public pain::Scene
{
public:
  MainScene() : pain::Scene() {}
  void init(pain::Application &app, Scene *scene, float aspectRatio, float zoom)
  {
    m_orthocamera = new pain::OrthoCameraEntity(scene, aspectRatio, zoom);
    pain::Renderer2d::init(*m_orthocamera);
    pain::NativeScriptComponent &a =
        m_orthocamera->getComponent<pain::NativeScriptComponent>();
    a.bind<pain::OrthoCameraController>();
    // ShapesController *sc = new ShapesController();
    // app.addImGuiInstance(sc);
    // m_sc = sc;

    m_texture = new pain::Texture("resources/textures/Checkerboard.png", true);

    // m_mainMap = std::make_unique<MainMap>(16.0f, 16.0f, tc.m_position, this);
    dummy = new Dummy(scene, {0.f, 0.f}, {1.f, 1.f}, {1.f, 1.f, 1.f, 1.f},
                      m_texture, 1.f);
    ls = &dummy->getComponent<pain::LuaScriptComponent>();
    ls->onCreate();
  }

  void onUpdate(double deltaTime) override
  {
    // ls->onUpdate(deltaTime);
    // PROFILE_FUNCTION()
    // pain::TransformComponent &tc =
    //     m_orthocamera->getComponent<pain::TransformComponent>();
    // m_mainMap->updateSurroundingChunks(tc.m_position, this);
  }
  void onRender(double currentTime) override
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
    // pain::Renderer2d::drawQuad({0.0f, 0.0f}, {0.4f, 0.4f}, m_texture, 1.0f,
    //                            {1.0f, 1.0f, 1.0f, 1.0f});
    // pain::Renderer2d::drawQuad({-0.5f, -0.5f}, {0.4f, 0.4f}, m_texture, 1.0f,
    //                            {1.0f, 1.0f, 1.0f, 1.0f});
  }
  void onEvent(const SDL_Event &event) override {}

private:
  std::vector<std::vector<int>> m_backgroundMap;
  pain::OrthoCameraEntity *m_orthocamera;
  std::shared_ptr<pain::Shader> m_texture_shader;
  pain::Texture *m_texture;
  Dummy *dummy;
  ImGuiController *m_sc;
  pain::LuaScriptComponent *ls;
};

pain::Application *pain::CreateApplication()
{
  LOG_T("Creating app");
  const char *title = "Developing Pain - Example 2d";
  const int width = 1280;
  const int height = 1000;
  Application *app = new Application(title, width, height);

  Scene *scene = new MainScene();
  ((MainScene *)scene)->init(*app, scene, (float)width / height, 1.0f);

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
  pain::Pain::initiate();
  pain::Pain::runApplication(pain::CreateApplication());
  return 0;
}
#endif
