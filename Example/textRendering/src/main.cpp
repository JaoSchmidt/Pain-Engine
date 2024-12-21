#include "imgui.h"
#include <pain.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

class ShapesController : public pain::ImGuiInstance
{
public:
  const void onImGuiUpdate() override
  {
    ImGui::Begin("Shapes Controller");
    // ImGui::Image((ImTextureID)m_font->getAtlasTexture().getRendererId(),
    //              {512, 512}, {0, 1}, {1, 0});
    ImGui::InputFloat("emShape", &m_emSize);
    if (ImGui::Button("Reset texture")) {
      m_font.reset(pain::Font::create(
          "resources/default/fonts/Epilogue-Black.ttf", m_emSize));
    }
    ImGui::End();
  }
  float m_emSize = 0;
  ShapesController(pain::Font *font) : m_font(font) {}
  std::unique_ptr<pain::Font> m_font;
};

class MainScene : public pain::Scene
{
public:
  MainScene() : pain::Scene() {}

  void init(pain::Application &app, Scene *scene, float aspectRatio, float zoom)
  {
    m_orthocamera =
        std::make_unique<pain::OrthoCameraEntity>(scene, aspectRatio, zoom);
    pain::Renderer2d::init(*m_orthocamera);
    auto &a = m_orthocamera->addComponent<pain::NativeScriptComponent>();
    a.bind<pain::OrthoCameraController>();
    ShapesController *sc = new ShapesController(
        pain::Font::create("resources/default/fonts/Epilogue-Black.ttf", 40.0));
    app.addImGuiInstance(sc);
    m_sc = sc;

    // *m_texture = m_font->getAtlasTexture();
  }
  void onUpdate(double deltaTimeSec) override {}
  void onRender(double currentTime) override
  {
    pain::Renderer2d::drawQuad({0.5f, 0.5f}, {0.3f, 0.3f}, {1.f, 1.f, 1.f, 1.f},
                               m_sc->m_font->getAtlasTexture(), 1.f);
    // pain::Renderer2d::drawString({-0.8f, 0.f}, "Hello World", *m_font.get(),
    //                              {1.f, 1.f, 1.f, 1.f});
  }
  void onEvent(const SDL_Event &event) override {}

private:
  ShapesController *m_sc;
  std::unique_ptr<pain::OrthoCameraEntity> m_orthocamera;
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
