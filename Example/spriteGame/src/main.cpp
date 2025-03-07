#include <pain.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

#include "initialMap.h"

class ShapesController : public pain::ImGuiInstance
{
public:
  const void onImGuiUpdate() override
  {
    ImGui::Begin("Shapes Controller");
    ImGui::End();
  }
};

class MainScene : public pain::Scene
{
public:
  MainScene() : pain::Scene(), m_mainMap(16.0f, 16.0f) {}
  void init(pain::Application &app, Scene *scene, float aspectRatio, float zoom)
  {
    m_orthocamera =
        std::make_unique<pain::OrthoCameraEntity>(scene, aspectRatio, zoom);
    pain::Renderer2d::init(*m_orthocamera);
    pain::NativeScriptComponent &a =
        m_orthocamera->addComponent<pain::NativeScriptComponent>(
            pain::NativeScriptComponent{});
    a.bind<pain::OrthoCameraController>();
    ShapesController *sc = new ShapesController();
    app.addImGuiInstance(sc);
    m_sc = sc;

    m_texture.reset(
        new pain::Texture("resources/textures/Checkerboard.png", true));
  }
  void onUpdate(double deltaTime) override {}
  void onRender(double currentTime) override
  {
    // m_orthocamera->onUpdate(deltaTime);
    const std::vector<std::vector<int>> mdm = m_mainMap.getDefaultMap();
    const std::vector<std::vector<int>> msm = m_mainMap.getSceneryMap();

    for (unsigned int i = 0; i < mdm[0].size(); i++) {
      for (unsigned int j = 0; j < mdm.size(); j++) {
        pain::Renderer2d::drawQuad(
            {1.f * i, -1.f * j}, {1.f, 1.f}, {1.0f, 1.0f, 1.0f, 1.0f},
            m_mainMap.getTexture(), 1.0f, m_mainMap.getTexCoord(mdm[j][i]));
      }
    }
    for (unsigned int i = 0; i < msm[0].size(); i++) {
      for (unsigned int j = 0; j < msm.size(); j++) {
        if (msm[j][i] != 00)
          pain::Renderer2d::drawQuad(
              {1.f * i, -1.f * j}, {1.f, 1.f}, {1.0f, 1.0f, 1.0f, 1.0f},
              m_mainMap.getTexture(), 1.0f, m_mainMap.getTexCoord(msm[j][i]));
      }
    }
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
  std::unique_ptr<pain::OrthoCameraEntity> m_orthocamera;
  std::shared_ptr<pain::Shader> m_texture_shader;
  std::shared_ptr<pain::Texture> m_texture;
  ShapesController *m_sc;
  MainMap m_mainMap;
};

pain::Application *pain::CreateApplication()
{
  LOG_T("Creating app");
  const char *title = "Developing Pain - Example 2d";
  const int width = 1280;
  const int height = 768;
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
