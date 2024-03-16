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
    // m_textureRectangle.reset(new pain::TextureRectangle());
    // m_texture.reset(new
    // pain::Texture("resources/textures/Checkerboard.png"));
    // m_texture_shader.reset(new
    // pain::Shader("resources/shaders/Texture.glsl"));
    // m_texture_shader->bind();
    // m_texture_shader->uploadUniformInt("u_Texture", 0);

    pain::Renderer2d::init(m_orthocamera);
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
    for (int i = 0; i < 1000; ++i) {
      // Generate random positions between -1.0 and 1.0
      float x = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);
      float y = -1.0f + static_cast<float>(rand()) / (RAND_MAX / 2.0f);

      // Generate random size between 0.1 and 0.3
      float size = 0.1f + static_cast<float>(rand()) / (RAND_MAX / 0.2f);

      // Generate random color components between 0.0 and 1.0
      float r = static_cast<float>(rand()) / RAND_MAX;
      float g = static_cast<float>(rand()) / RAND_MAX;
      float b = static_cast<float>(rand()) / RAND_MAX;
      float a = 1.0f; // Alpha component (opacity), set to 1.0 for fully opaque

      glm::vec2 position(x, y);
      glm::vec2 quadSize(size, size);
      glm::vec4 color(r, g, b, a);

      pain::Renderer2d::drawQuad(position, quadSize, color);
    }
    pain::Renderer2d::endScene();
    // glm::mat4 transform =
    //     glm::translate(glm::mat4(1.0f), m_textureRectangle->getPos()) *
    //     glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
    // m_texture->bind();
    // pain::Renderer2d::submit(m_texture_shader,
    //                          m_textureRectangle->getVertexArr(), transform);
    // pain::Renderer2d::endScene(m_textureRectangle->getVertexArr());

    // pain::Renderer2d::submit(
    //     m_isocelesTriangle->getShader(), m_isocelesTriangle->getVertexArr(),
    //     glm::translate(glm::mat4(1.0f), m_isocelesTriangle->getPos()));
  }
  void onEvent(const SDL_Event &event) override
  {
    m_orthocamera->onEvent(event);
  }
  void onDetach() override { LOG_I("Layer attached to the stack"); }
  void onAttach() override { LOG_I("Layer dettached from the stack"); }

private:
  std::shared_ptr<pain::Shader> m_texture_shader;
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
