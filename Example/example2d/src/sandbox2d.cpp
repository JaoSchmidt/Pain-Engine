#include <pain.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

class MainLayer : public pain::Layer
{
public:
  MainLayer() : Layer("main")
  {
    m_orthocamera.reset(new pain::OrthographicCameraController(800.0 / 600.0));
    pain::Renderer2d::init(m_orthocamera);
  }

  void onUpdate(pain::DeltaTime deltaTime) override
  {
    const double dtSeconds = deltaTime.GetSeconds();

    m_orthocamera->onUpdate(dtSeconds);

    pain::Renderer2d::setClearColor(
        glm::vec4(0.5294117, 0.807843137, 0.921568627, 1));
    pain::Renderer2d::clear();

    pain::Renderer2d::beginScene();
    pain::Renderer2d::drawQuad({0.0f, 0.0f}, {0.2f, 0.2f},
                               {0.8f, 0.9f, 0.3f, 1.0f});
    pain::Renderer2d::drawQuad({-0.5f, 0.0f}, {0.3f, 0.3f},
                               {0.8f, 0.9f, 0.3f, 1.0f});
    pain::Renderer2d::drawQuad({-0.5f, -0.5f}, {0.3f, 0.3f},
                               {0.8f, 0.9f, 0.3f, 1.0f});

    pain::Renderer2d::endScene();
  }
  void onEvent(const SDL_Event &event) override
  {
    m_orthocamera->onEvent(event);
  }
  void onDetach() override { LOG_I("Layer attached to the stack"); }
  void onAttach() override { LOG_I("Layer dettached from the stack"); }

private:
  // std::shared_ptr<pain::Shader> m_texture_shader;
  std::shared_ptr<pain::OrthographicCameraController> m_orthocamera;
  // std::shared_ptr<pain::TextureRectangle> m_textureRectangle;
  std::shared_ptr<pain::Texture> m_texture;
};

class Sandbox : public pain::Application
{
public:
  Sandbox(const char *title, int w, int h) : Application(title, w, h)
  {
    pushLayer(new MainLayer());
  }

  ~Sandbox() {}
};

pain::Application *pain::CreateApplication()
{
  LOG_T("Creating app");
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
