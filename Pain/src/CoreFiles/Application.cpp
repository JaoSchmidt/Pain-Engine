#include "Application.h"
#include "BufferLayout.h"
#include "LogWrapper.h"
#include "external/SDL/include/SDL3/SDL_keyboard.h"

const unsigned int NUM_FLOATS_PER_VERTICE = 6;
const unsigned int VERTEX_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

namespace pain
{
/* Creates window, opengl context and init glew*/
void Application::initialSetup(const char *title, int w, int h)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    PLOG_E("SDL could not be init", SDL_GetError());
  } else {
    PLOG_T("SDL video is initialized");
  }
  m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
  // m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  // if (m_window != nullptr && m_renderer != nullptr) {
  //   PLOG_T("Application window and render initialized");
  // } else {
  if (m_window == nullptr)
    PLOG_E("Application window not initialized");
  //   if (m_renderer == nullptr)
  //     PLOG_E("Renderer not initialized");
  // }
  m_context = SDL_GL_CreateContext(m_window);

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    PLOG_E("Error: {}", std::string(reinterpret_cast<const char *>(
                            glewGetErrorString(err))));
    exit(1);
  }
  PLOG_T("GLEW version: {}", std::string(reinterpret_cast<const char *>(
                                 glewGetString(GLEW_VERSION))));
  PLOG_T("GL version: {}",
         std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION))));
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glErrorHandler, 0);
}

Application::Application(const char *title, int w, int h) : m_maxFrameRate(60)
{
  initialSetup(title, w, h);
  m_layerStack = new LayerStack();
  m_vertexArray.reset(new VertexArray());
  float vertices[3 * 7] = {
      -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f, //
      0.5f,  -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, //
      0.0f,  0.5f,  0.0f, 0.8f, 0.8f, 0.2f, 1.0f  //
  };

  m_vertexBuffer.reset(new VertexBuffer(vertices, sizeof(vertices)));

  BufferLayout layout = {
      {ShaderDataType::Float3, "a_Position"}, //
      {ShaderDataType::Float4, "a_Color"}     //
  };

  m_vertexBuffer->setLayout(layout);
  m_vertexArray->addVertexBuffer(m_vertexBuffer);

  uint32_t indices[3] = {0, 1, 2};
  m_indexBuffer.reset(
      new IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));

  m_vertexArray->setIndexBuffer(m_indexBuffer);

  std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
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

  m_shader.reset(new Shader(vertexSrc, fragmentSrc));
}

Application::~Application()
{
  SDL_GL_DeleteContext(m_context);
  delete m_layerStack;
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void Application::pushLayer(Layer *layer) { m_layerStack->pushLayer(layer); }

void Application::popLayer(Layer *layer) { m_layerStack->popLayer(layer); }

void Application::stop() { m_isGameRunning = false; }

void Application::handleEvents()
{
  SDL_Event event;

  // (1) Handle Input
  // Start our event loop
  while (SDL_PollEvent(&event)) {
    // Handle each specific event
    switch (event.type) {

    case SDL_QUIT:
      stop();
      break;
    case SDL_KEYDOWN:
      PLOG_I("key pressed: {}", SDL_GetKeyName(event.key.keysym.sym));
      break;
    default:
      break;
    }
  }
}

void Application::handleUpdate()
{
  for (auto pLayer = m_layerStack->end(); pLayer != m_layerStack->begin();) {
    (*--pLayer)->onUpdate();
  }
}

void Application::handleRender()
{
  glClearColor(0.5294117, 0.807843137, 0.921568627, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  m_shader->bind();
  // m_vertexArray->bind();
  glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, 0);

  SDL_GL_SwapWindow(m_window);
}

void Application::run()
{
  while (m_isGameRunning) {
    uint32_t start = SDL_GetTicks();
    // unsigned int buttons;
    // buttons = SDL_GetMouseState(&m_mouseX, &m_mouseY);

    // handle events first
    handleEvents();
    // handle any updates
    handleUpdate();

    // hanlde our rendering
    handleRender();

    uint32_t elapsedTime = SDL_GetTicks() - start;
    if (elapsedTime < m_maxFrameRate) {
      SDL_Delay(m_maxFrameRate - elapsedTime);
    }
  };
}

void Application::glErrorHandler(unsigned int source, unsigned int type,
                                 unsigned int id, unsigned int severity,
                                 int lenght, const char *message,
                                 const void *userParam)
{

  PLOG_W("---------------");
  PLOG_W("Debug message ({}): {}", id, message);
  // clang-format off
    switch (source) 
    { 
        case GL_DEBUG_SOURCE_API:             PLOG_W(  "Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   PLOG_W(  "Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: PLOG_W(  "Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     PLOG_W(  "Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:     PLOG_W(  "Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:           PLOG_W(  "Source: Other"); break;
    } 

    switch (type) 
    { 
        case GL_DEBUG_TYPE_ERROR:               PLOG_E(  "Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: PLOG_W(  "Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  PLOG_W(  "Type: Undefined Behaviour"); break; 
        case GL_DEBUG_TYPE_PORTABILITY:         PLOG_W(  "Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         PLOG_W(  "Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:              PLOG_W(  "Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          PLOG_W(  "Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           PLOG_W(  "Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:               PLOG_W(  "Type: Other"); break;
    } 
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         PLOG_E(  "Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       PLOG_W(  "Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW:          PLOG_W(  "Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: PLOG_W(  "Severity: notification"); break;
    }
	if (GL_DEBUG_SEVERITY_HIGH == type || GL_DEBUG_TYPE_ERROR == type){
		P_ASSERT(false,"OpenGL critical error");
	}
  // clang-format on
}
} // namespace pain
