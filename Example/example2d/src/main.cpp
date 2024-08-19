#include <pain.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

class ShapesController : public pain::ImGuiInstance
{
public:
  const void onImGuiUpdate() override
  {
    // ImGui::Begin("Shapes Controller");
    // ImGui::Text("First Quad");
    // ImGui::InputFloat2("Position", m_vecPos);
    // ImGui::InputFloat2("Size", m_vecSize);
    // ImGui::ColorEdit4("Color", m_vecColor);
    // ImGui::End();
  }

  float m_vecPos[2] = {0.0f, -0.80f};
  float m_vecSize[2] = {0.3f, 0.3f};
  float m_vecColor[4] = {0.9f, 0.3f, 0.2f, 1.0f};
};

class MainScene : public pain::Scene
{
public:
  MainScene() : pain::Scene() {}
  void init(std::shared_ptr<pain::OrthoCameraEntity> pCamera,
            ShapesController *sc)
  {
    m_orthocamera = pCamera;
    pain::Renderer2d::init(m_orthocamera);
    auto &a = m_orthocamera->addComponent<pain::NativeScriptComponent>();
    a.bind<pain::OrthoCameraController>();
    m_texture.reset(new pain::Texture("resources/textures/Checkerboard.png"));

    m_sc = sc;
    // m_rect1.reset(new pain::RectangleSprite(this, {0.0f, 0.0f}, {0.4f, 0.4f},
    //                                         {1.0f, 1.0f, 1.0f, 1.0f},
    //                                         m_texture, 1.0f));
    // m_rect2.reset(new pain::RectangleSprite(this, {-0.5f, -0.5f}, {0.4f,
    // 0.4f},
    //                                         {1.0f, 1.0f, 1.0f, 1.0f},
    //                                         m_texture, 1.0f));
    // m_rect3.reset(new pain::Rectangle(this, {0.0f, -0.8f}, {0.3f, 0.3f},
    //                                   {0.9f, 0.3f, 0.2f, 1.0f}));
    // m_rect4.reset(new pain::Rectangle(this, {-0.5f, 0.0f}, {0.3f, 0.3f},
    //                                   {0.8f, 0.9f, 0.3f, 1.0f}));
  }
  void onRender() override
  {
    // m_ImGuiTest->onUpdateImGui();
    const auto &pos = m_sc->m_vecPos;
    const auto &size = m_sc->m_vecSize;
    const auto &col = m_sc->m_vecColor;
    pain::Renderer2d::drawQuad({pos[0], pos[1]}, {size[0], size[1]},
                               {col[0], col[1], col[2], col[3]});
    pain::Renderer2d::drawQuad({-0.5f, 0.0f}, {0.3f, 0.3f},
                               {0.8f, 0.9f, 0.3f, 1.0f});
    // pain::Renderer2d::drawQuad({0.0f, 0.0f}, {0.4f, 0.4f},
    //                            {1.0f, 1.0f, 1.0f, 1.0f}, m_texture, 1.0f);
    // pain::Renderer2d::drawQuad({0.5f, -0.5f}, {0.4f, 0.4f},
    //                            {1.0f, 1.0f, 1.0f, 1.0f}, m_texture, 1.0f);

    pain::Renderer2d::drawTri({-0.5f, -0.5f}, {0.5f, 0.5f},
                              {0.2f, 0.9f, 0.8f, 1.0f});
    pain::Renderer2d::drawRotQuad({-0.6f, 0.8f}, {0.4f, 0.4f},
                                  {1.0f, 1.0f, 1.0f, 1.0f}, m_texture, 1.0f,
                                  m_radians);
    pain::Renderer2d::drawRotQuad({0.7f, 0.7f}, {0.3f, 0.3f},
                                  {0.8f, 0.9f, 0.3f, 1.0f}, m_radians);
  }
  void onUpdate(double deltaTimeSec) override
  {
    m_radians = m_radians + std::fmod(10.0f * deltaTimeSec, 2 * 3.14159265359f);
  }
  void onEvent(const SDL_Event &event) override
  {
    // m_orthocamera->onEvent(event);
  }

private:
  float m_radians = 0.0f;
  ShapesController *m_sc;
  std::shared_ptr<pain::Shader> m_texture_shader;
  std::shared_ptr<pain::OrthoCameraEntity> m_orthocamera;
  std::shared_ptr<pain::RectangleSprite> m_rect1;
  std::shared_ptr<pain::RectangleSprite> m_rect2;
  std::shared_ptr<pain::Rectangle> m_rect3;
  std::shared_ptr<pain::Rectangle> m_rect4;
  std::shared_ptr<pain::Texture> m_texture;
  std::shared_ptr<pain::Texture> m_spriteSheet;
};

class Sandbox : public pain::Application
{
public:
  Sandbox(const char *title, int w, int h) : Application(title, w, h)
  {
    pain::Scene *scene = new MainScene();

    std::shared_ptr<pain::OrthoCameraEntity> pCamera;
    pCamera.reset(new pain::OrthoCameraEntity(scene, (float)w / h, 1.0f));
    ShapesController *sc = new ShapesController();
    ((MainScene *)scene)->init(pCamera, sc);
    addImGuiInstance(sc);

    pushScene("main", scene);
    attachScene("main");
  }

  ~Sandbox() {}
};

pain::Application *pain::CreateApplication()
{
  LOG_T("Creating app");
  const char *title = "Developing Pain - Example 2d";
  const int width = 800;
  const int height = 600;
  // const int width = 1280;
  // const int height = 720;
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
