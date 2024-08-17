#include "CoreFiles/Application.h"
#include <pain.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>

class MainLayer : public pain::Scene
{
public:
  MainLayer() : Scene()
  {
    m_cameraController.reset(
        new pain::PerspectiveCameraController(800.0, 600.0, 60.0f));
    pain::Renderer3d::init(m_cameraController);
  }

  void onUpdate(pain::DeltaTime deltaTime) override
  {
    const double dtSeconds = deltaTime.GetSeconds();
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_LEFT])
      m_textureRectangle->mvPositiveX(dtSeconds);
    if (state[SDL_SCANCODE_RIGHT])
      m_textureRectangle->mvNegativeX(dtSeconds);

    m_cameraController->onUpdate(dtSeconds);
    const glm::vec4 dark_grey = {0.2, 0.2, 0.2, 1};
    // const glm::vec4 sky_blue = {0.5294117, 0.807843137, 0.921568627, 1};
    pain::Renderer3d::setClearColor(dark_grey);
    pain::Renderer3d::clear();

    pain::Renderer3d::beginScene();
    // pain::Renderer3d::drawSimpleCube();
    pain::Renderer3d::drawCube({0.0f, 0.0f, 0.0f}, {0.2f, 0.2f, 0.2f},
                               {0.8f, 0.9f, 0.3f, 1.0f});
    pain::Renderer3d::drawCube({-0.5f, 0.0f, 0.5f}, {0.3f, 0.3f, 0.3f},
                               {0.8f, 0.9f, 0.3f, 1.0f});
    pain::Renderer3d::drawCube({-0.5f, -0.5f, -0.5}, {0.3f, 0.3f, 0.3f},
                               {0.8f, 0.9f, 0.3f, 1.0f});
    pain::Renderer3d::endScene();
  }

  void onEvent(const SDL_Event &event) override
  {
    m_cameraController->onEvent(event);
  }
  void onDetach() override {}
  // LOG_I("Layer attached to the stack");

  void onAttach() override {}
  // LOG_I("Layer dettached from the stack");

private:
  std::shared_ptr<pain::Shader> m_texture_shader;
  std::shared_ptr<pain::PerspectiveCameraController> m_cameraController;
  std::shared_ptr<pain::TextureRectangle> m_textureRectangle;
  std::shared_ptr<pain::Texture> m_texture;
};

class Sandbox : public pain::Application
{
public:
  Sandbox(const char *title, int w, int h) : Application(title, w, h)
  {
    pain::Scene *scene = new MainLayer();
    pushScene("main", scene);
    attachScene("main");
    std::shared_ptr<pain::PerspectiveCamera> pCamera =
        std::make_shared<pain::OrthoCameraEntity>(scene, (float)w / h);
    ((MainScene *)scene)->initCamera(pCamera);
  }

  ~Sandbox() {}
};

pain::Application *CreateApplication()
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
  pain::Pain::runApplication(CreateApplication());
  return 0;
}
#else
int main(int argc, char *argv[])
{
  pain::Pain::initiate();
  pain::Pain::runApplication(CreateApplication());
  return 0;
}
#endif
