#include "CoreFiles/Application.h"
#include "Core.h"
#include "CoreFiles/ImGuiController.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "SDL_timer.h"
#include "glm/fwd.hpp"
#include <SDL2/SDL_version.h>
#include <cmath>
#include <thread>

namespace pain
{
unsigned Application::getProcessorCount()
{
  return std::thread::hardware_concurrency();
}

/* Creates window, opengl context and init glew*/
Application::Application(const char *title, int w, int h)
{
  // =========================================================================//
  // SDL Initial setup
  // =========================================================================//
  P_ASSERT(
      SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) >= 0,
      "SDL video could not be init {}", SDL_GetError());
  PLOG_T("SDL video is initialized");

#ifdef SDL_HINT_IME_SHOW_UI
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  m_window = SDL_CreateWindow(
      title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
  if (m_window == nullptr)
    PLOG_E("Application window not initialized");
  m_context = SDL_GL_CreateContext(m_window);

  SDL_version sdl_version;
  SDL_GetVersion(&sdl_version);
  PLOG_T("SDL version: {}.{}.{}", sdl_version.major, sdl_version.minor,
         sdl_version.patch);

  // =========================================================================//
  //
  // =========================================================================//
  // =========================================================================//
  // OpenGL Initial setup
  // =========================================================================//

  int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
  if (version == 0) {
    PLOG_E("Error: Failed to initialize glad");
    exit(1);
  }
  PLOG_T("GL version: {}",
         std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION))));
  // PLOG_T("Default relative path is: {}",
  //        std::filesystem::current_path().string());

  int versionMajor;
  int versionMinor;
  glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
  glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
  P_ASSERT(versionMajor >= 4 && versionMinor >= 3,
           "OpenGL version must be above 4.3, current version is {}.{}",
           versionMajor, versionMinor);

  int maxTextureUnits;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
  PLOG_T("GPU Texture Mapping Units: {}", maxTextureUnits);

#ifndef NDEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glErrorHandler, 0);
#endif
  // =========================================================================//
  // Application Initial setup
  // =========================================================================//
  m_imguiController = std::make_unique<ImGuiController>();
  m_isMinimized = false;
  m_sceneManager = new pain::SceneManager();
  m_defaultImGuiInstance = new EngineController();
  m_imguiController->addImGuiMenu(m_defaultImGuiInstance);
  // SDL_SetWindowGrab(m_window, SDL_TRUE);
  // SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
}

void Application::stop() { m_isGameRunning = false; }

void Application::run()
{
  DeltaTime deltaTime;
  double accumulator = 0.0;
  // double renderAccumulator = 0.0;
  bool hasAlteredMultiplier = false;

  DeltaTime lastFrameTime = SDL_GetPerformanceCounter();
  while (m_isGameRunning) {
    uint64_t start = SDL_GetPerformanceCounter();
    deltaTime = start - lastFrameTime;
    lastFrameTime = start;

    // =============================================================== //
    // Calculate FPS sample
    // =============================================================== //

    // The purpose of this is to calculate the fps inside as an average of the
    // previous "FPS_SAMPLE_COUNT" measurements
    m_fpsSamples[m_currentSample] =
        SDL_GetPerformanceFrequency() / deltaTime.GetNanoseconds();
    m_currentSample = (m_currentSample + 1) % FPS_SAMPLE_COUNT;
    if (m_currentSample % 64 == 0) { // update displayed fps
      m_defaultImGuiInstance->m_currentTPS = 0.0;
      for (const double fpsSample : m_fpsSamples) {
        m_defaultImGuiInstance->m_currentTPS += fpsSample;
      }
      m_defaultImGuiInstance->m_currentTPS /= FPS_SAMPLE_COUNT;
      hasAlteredMultiplier = false;
    }
    // =============================================================== //
    // Handle Updates
    // =============================================================== //
    // NOTE: Eventually render and update systems need to execute at different
    // rates...

    {
      PROFILE_SCOPE("Application::run - Handle Updates");
      double deltaSeconds = deltaTime.GetSeconds() * m_timeMultiplier;
      if (m_isSimulation && !hasAlteredMultiplier) {
        // the purpose of this is to only increase when there is spare tps
        if (m_defaultImGuiInstance->m_currentTPS > 60.0)
          m_timeMultiplier *= 1.10;
        else
          m_timeMultiplier *= 0.90;
        hasAlteredMultiplier = true;
      }
      // Uncomment this to create an accumulator cap
      // accumulator = fmod(accumulator + deltaSeconds, 15.0);
      accumulator += deltaSeconds;

      // renderAccumulator += deltaSeconds;
      while (accumulator >= m_fixedUpdateTime) {
        for (auto pScene = m_sceneManager->begin();
             pScene != m_sceneManager->end(); ++pScene) {
          (*pScene)->onUpdate(m_fixedUpdateTime);
          (*pScene)->updateSystems(m_fixedUpdateTime);
        }
        accumulator -= m_fixedUpdateTime;
      }
    }

    // =============================================================== //
    // Handle Events
    // =============================================================== //
    {
      PROFILE_SCOPE("Application::run - Handle Events");
      SDL_Event event;
      // Start our event loop (goes until the queue is free)
      while (SDL_PollEvent(&event)) {
        m_imguiController->onEvent(event);
        switch (event.type) {
        case SDL_QUIT:
          stop();
          break;
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
              event.window.windowID == SDL_GetWindowID(m_window))
            stop();
          else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
            m_isMinimized = true;
          else if (event.window.event == SDL_WINDOWEVENT_RESTORED)
            m_isMinimized = false;
          break;
        default:
          break;
        }
        // Handle each specific event
        for (auto pScene = m_sceneManager->begin();
             pScene != m_sceneManager->end(); ++pScene) {
          (*pScene)->updateSystems(event);
          (*pScene)->onEvent(event);
        }
      }
      m_imguiController->onUpdate(m_isMinimized);
    }
    // =============================================================== //
    // Handle rendering
    // =============================================================== //
    {
      PROFILE_SCOPE("Application::run - Handle Rendering");
      if ((!m_isMinimized) && m_isRendering) {
        // int w, h;
        // SDL_GetWindowSize(m_window, &w, &h);
        // Renderer2d::setViewport(0, 0, w, h);
        Renderer2d::setClearColor(m_clearColor);
        Renderer2d::clear();

        double globalTime = lastFrameTime.GetSeconds();
        for (auto pScene = m_sceneManager->begin();
             pScene != m_sceneManager->end(); ++pScene) {
          Renderer2d::beginScene(globalTime);
          (*pScene)->onRender(globalTime);
          (*pScene)->renderSystems(globalTime);
          Renderer2d::endScene();
        }
        m_imguiController->onRender();
        P_ASSERT(m_window != nullptr, "m_window is nullptr")
        SDL_GL_SwapWindow(m_window);
      } else {
        P_ASSERT(m_window != nullptr, "m_window is nullptr")
        Renderer2d::setClearColor(glm::vec4(0.f, 0.f, 0.f, 1.f));
        Renderer2d::clear();
        m_imguiController->onRender();
        SDL_GL_SwapWindow(m_window);
      }
    }

    // =============================================================== //
    // Fix clock if it's too ahead
    // =============================================================== //
    uint64_t end = SDL_GetPerformanceCounter();
    double frameDuration =
        static_cast<double>(end - start) / SDL_GetPerformanceFrequency();
    if (frameDuration < m_fixedFPS) {
      SDL_Delay(static_cast<uint32_t>((m_fixedFPS - frameDuration) * 1000.0));
    }
  };
}

Application::~Application()
{
  SDL_GL_DeleteContext(m_context);
  delete m_sceneManager;
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void Application::glErrorHandler(unsigned int source, unsigned int type,
                                 unsigned int id, unsigned int severity,
                                 int lenght, const char *message,
                                 const void *userParam)
{
  // uncomment this if your gpu being too educated with warnings
  // notification warnings are usually just optimizations anyway
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

  if (GL_DEBUG_SEVERITY_HIGH == type || GL_DEBUG_TYPE_ERROR == type) {
    PLOG_E("-----------------------------------");
    PLOG_E("Debug message ({}): {}", id, message);
  } else {
    PLOG_W("-----------------------------------");
    PLOG_W("Debug message ({}): {}", id, message);
  }
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
	if (GL_DEBUG_SEVERITY_HIGH == type || GL_DEBUG_TYPE_ERROR == type)
		P_ASSERT(false,"OpenGL critical error");
  // clang-format on
}
} // namespace pain
