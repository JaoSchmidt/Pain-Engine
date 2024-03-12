#include "Misc/CameraController.h"
#include <SDL2/SDL_events.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <pain.h>

class MainLayer : public pain::Layer
{
public:
  MainLayer() : Layer("main")
  {
    m_orthocamera.reset(new pain::OrthographicCameraController(800.0 / 600.0));
    // m_isocelesTriangle.reset(new pain::IsocelesTriangle(1.0f, 0.5f));
    m_textureRectangle.reset(new pain::TextureRectangle());
    m_texture.reset(new pain::Texture("resources/textures/Checkerboard.png"));
    m_textureRectangle->getShader()->bind();
    m_textureRectangle->getShader()->uploadUniformInt("u_Texture", 0);

    pain::Renderer::beginScene(m_orthocamera);
  }

  void onUpdate(pain::DeltaTime deltaTime) override
  {
    const double dtSeconds = deltaTime.GetSeconds();
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_LEFT])
      m_textureRectangle->mvPositiveX(dtSeconds);
    if (state[SDL_SCANCODE_RIGHT])
      m_textureRectangle->mvNegativeX(dtSeconds);

    m_orthocamera->onUpdate(dtSeconds);
    pain::Renderer::setClearColor(
        glm::vec4(0.5294117, 0.807843137, 0.921568627, 1));
    pain::Renderer::clear();

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), m_textureRectangle->getPos()) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
    m_texture->bind();
    pain::Renderer::submit(m_textureRectangle->getShader(),
                           m_textureRectangle->getVertexArr(), transform);
    pain::Renderer::endScene(m_textureRectangle->getVertexArr());

    // pain::Renderer::submit(
    //     m_isocelesTriangle->getShader(), m_isocelesTriangle->getVertexArr(),
    //     glm::translate(glm::mat4(1.0f), m_isocelesTriangle->getPos()));

    // pain::Renderer::endScene(m_isocelesTriangle->getVertexArr());
  }
  void onEvent(const SDL_Event &event) override
  {
    m_orthocamera->onEvent(event);
  }
  void onDetach() override { LOG_I("Layer attached to the stack"); }
  void onAttach() override { LOG_I("Layer dettached from the stack"); }

private:
  std::shared_ptr<pain::OrthographicCameraController> m_orthocamera;
  std::shared_ptr<pain::TextureRectangle> m_textureRectangle;
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
