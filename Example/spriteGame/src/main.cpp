#include <pain.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

#include <iostream>

class MainScene : public pain::Scene
{
public:
  MainScene() : pain::Scene() {}
  void init(std::shared_ptr<pain::OrthoCameraEntity> pCamera)
  {
    m_orthocamera = pCamera;
    pain::Renderer2d::init(m_orthocamera);
    m_texture.reset(new pain::Texture("resources/textures/Checkerboard.png"));
    m_rect1.reset(new pain::RectangleSprite(this, {0.0f, -0.8f}, {0.3f, 0.3f},
                                            m_texture, 1.0f,
                                            {0.9f, 0.3f, 0.2f, 1.0f}));
  }

  void onUpdate(double deltaTime) override
  {
    m_orthocamera->onUpdate(deltaTime);

    // pain::Renderer2d::beginScene();
    // pain::Renderer2d::drawQuad({0.0f, -0.8f}, {0.3f, 0.3f},
    //                            {0.9f, 0.3f, 0.2f, 1.0f});
    // pain::Renderer2d::drawQuad({-0.5f, 0.0f}, {0.3f, 0.3f},
    //                            {0.8f, 0.9f, 0.3f, 1.0f});
    // pain::Renderer2d::drawQuad({0.0f, 0.0f}, {0.4f, 0.4f}, m_texture, 1.0f,
    //                            {1.0f, 1.0f, 1.0f, 1.0f});
    // pain::Renderer2d::drawQuad({-0.5f, -0.5f}, {0.4f, 0.4f}, m_texture, 1.0f,
    //                            {1.0f, 1.0f, 1.0f, 1.0f});
    // pain::Renderer2d::endScene();
  }
  void onEvent(const SDL_Event &event) override
  {
    m_orthocamera->onEvent(event);
  }

private:
  std::shared_ptr<pain::Shader> m_texture_shader;
  std::shared_ptr<pain::OrthoCameraEntity> m_orthocamera;
  std::shared_ptr<pain::RectangleSprite> m_rect1;
  // std::shared_ptr<pain::RectangleSprite> m_rect2;
  // std::shared_ptr<pain::TextureRectangle> m_rect3;
  // std::shared_ptr<pain::TextureRectangle> m_rect4;
  std::shared_ptr<pain::Texture> m_texture;
};

class Sandbox : public pain::Application
{
public:
  Sandbox(const char *title, int w, int h) : Application(title, w, h)
  {
    pain::Scene *scene = new MainScene();
    std::shared_ptr<pain::OrthoCameraEntity> pCamera =
        std::make_shared<pain::OrthoCameraEntity>(scene, (float)w / h);
    ((MainScene *)scene)->init(pCamera);
    pushScene("main", scene);
    attachScene("main");
  }

  ~Sandbox() {}
};

pain::Application *pain::CreateApplication()
{
  // LOG_T("Creating app");
  const char *title = "Developing Pain";
  const int width = 800;
  const int height = 600;
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
