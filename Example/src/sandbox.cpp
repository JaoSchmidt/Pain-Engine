#include "DeltaTime.h"
#include <SDL2/SDL_events.h>
#include <pain.h>

class MainLayer : public pain::Layer
{
public:
  MainLayer()
      : Layer("main"), m_cameraPosistion(0.0f, 0.0f, 0.0f), m_cameraSpeed(1.0f)
  {
    m_orthocamera.reset(new pain::OrthographicCamera(-1.0f, 1.0f, -0.9f, 0.9f));
    m_vertexArray.reset(new pain::VertexArray());
    float vertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f, //
        0.5f,  -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, //
        0.0f,  0.5f,  0.0f, 0.8f, 0.8f, 0.2f, 1.0f  //
    };

    m_vertexBuffer.reset(new pain::VertexBuffer(vertices, sizeof(vertices)));

    pain::BufferLayout layout = {
        {pain::ShaderDataType::Float3, "a_Position"}, //
        {pain::ShaderDataType::Float4, "a_Color"}     //
    };

    m_vertexBuffer->setLayout(layout);
    m_vertexArray->addVertexBuffer(m_vertexBuffer);

    uint32_t indices[3] = {0, 1, 2};
    m_indexBuffer.reset(
        new pain::IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));

    m_vertexArray->setIndexBuffer(m_indexBuffer);

    std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
			}
		)";

    std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec4 v_Color;
			in vec3 v_Position;

			void main()
			{
				color = v_Color;
			}
		)";

    m_shader.reset(new pain::Shader(vertexSrc, fragmentSrc));
    pain::Renderer::beginScene(m_orthocamera);
  }
  void onUpdate(pain::DeltaTime deltaTime) override
  {
    const double dtSeconds = deltaTime.GetSeconds();
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W])
      m_cameraPosistion.y += m_cameraSpeed * dtSeconds;
    if (state[SDL_SCANCODE_S])
      m_cameraPosistion.y -= m_cameraSpeed * dtSeconds;

    if (state[SDL_SCANCODE_D])
      m_cameraPosistion.x += m_cameraSpeed * dtSeconds;
    if (state[SDL_SCANCODE_A])
      m_cameraPosistion.x -= m_cameraSpeed * dtSeconds;

    m_orthocamera->SetPosition(m_cameraPosistion);

    pain::Renderer::setClearColor(
        glm::vec4(0.5294117, 0.807843137, 0.921568627, 1));
    pain::Renderer::clear();

    pain::Renderer::submit(m_shader, m_vertexArray);

    pain::Renderer::endScene();
  }
  void onEvent(const SDL_Event &event) override {}
  void onDetach() override { LOG_I("Layer attached to the stack"); }
  void onAttach() override { LOG_I("Layer dettached from the stack"); }

private:
  std::shared_ptr<pain::VertexArray> m_vertexArray;
  std::shared_ptr<pain::Shader> m_shader;
  std::shared_ptr<pain::VertexBuffer> m_vertexBuffer;
  std::shared_ptr<pain::IndexBuffer> m_indexBuffer;
  std::shared_ptr<pain::Camera> m_orthocamera;
  glm::vec3 m_cameraPosistion;
  float m_cameraSpeed;
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
