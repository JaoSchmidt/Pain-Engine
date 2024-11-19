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
    // ImGui::Image((ImTextureID)m_font.getAtlasTexture().getRendererId(),
    //              {512, 512}, {0, 1}, {1, 0});
    ImGui::End();
  }
  // ShapesController(pain::Font &font) : m_font(font) {}
  // pain::Font &m_font;
};

class MainScene : public pain::Scene
{
public:
  MainScene()
      : pain::Scene(),
        m_font(std::make_shared<pain::Font>("resources/fonts/Arial.ttf"))
  {
  }
  void init(std::shared_ptr<pain::OrthoCameraEntity> pCamera)
  {
    m_orthocamera = pCamera;
    pain::Renderer2d::init(m_orthocamera);
    auto &a = m_orthocamera->addComponent<pain::NativeScriptComponent>();
    a.bind<pain::OrthoCameraController>();

    ShapesController *sc = new ShapesController();
    pain::Application::Get().addImGuiInstance(sc);
    m_sc = sc;

    m_textureAtlas = std::make_shared<pain::Texture>(m_font->getAtlasTexture());
  }
  void onUpdate(double deltaTimeSec) override {}
  void onRender(double currentTime) override
  {
    // pain::Renderer2d::drawQuad({0.5f, 0.5f}, {0.3f, 0.3f},
    //                            {0.8f, 0.9f, 0.3f, 1.0f},
    //                            m_textureAtlas, 1.f);
    pain::Renderer2d::drawString({-0.8f, 0.f}, "Hello World", *m_font.get(),
                                 {1.f, 1.f, 1.f, 1.f}, m_textureAtlas);
  }
  void onEvent(const SDL_Event &event) override {}

private:
  std::shared_ptr<pain::Texture> m_textureAtlas;
  std::shared_ptr<pain::Font> m_font;
  ShapesController *m_sc;
  std::shared_ptr<pain::OrthoCameraEntity> m_orthocamera;
};

class Sandbox : public pain::Application
{
public:
  Sandbox(const char *title, int w, int h) : Application(title, w, h)
  {
    pain::Scene *scene = new MainScene();

    std::shared_ptr<pain::OrthoCameraEntity> pCamera;
    pCamera.reset(new pain::OrthoCameraEntity(scene, (float)w / h, 1.0f));
    ((MainScene *)scene)->init(pCamera);

    pushScene("main", scene);
    attachScene("main");
  }

  ~Sandbox() {}
};

pain::Application *pain::CreateApplication()
{
  LOG_T("Creating app");
  const char *title = "Developing Pain - Example 2d";
  // const int width = 800;
  // const int height = 600;
  const int width = 1280;
  const int height = 768;
  return new Sandbox(title, width, height);
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
