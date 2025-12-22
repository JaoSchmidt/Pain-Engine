#include "CoreFiles/Application.h"
#include "Assets/DefaultTexture.h"
#include "Assets/ResourceManager.h"
#include "ContextBackend.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "GUI/ImGuiSys.h"
#include "Scripting/State.h"
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_version.h>
#include <memory>
#include <thread>

namespace pain
{
unsigned Application::getProcessorCount()
{
  return std::thread::hardware_concurrency();
}

Application *Application::createApplication(const char *title, int w, int h)
{
  PLOG_T(resources::getCurrentWorkingDir());
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

  SDL_Window *window = SDL_CreateWindow(
      title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
  if (window == nullptr)
    PLOG_E("Application window not initialized");
  void *context = SDL_GL_CreateContext(window);

  SDL_version sdl_version;
  SDL_GetVersion(&sdl_version);
  PLOG_T("SDL version: {}.{}.{}", sdl_version.major, sdl_version.minor,
         sdl_version.patch);

  backend::Init();
  // =========================================================================//
  // Application Initial setup before
  // =========================================================================//
  sol::state luaState = createLuaState();

  // SDL_SetWindowGrab(m_window, SDL_TRUE);
  // SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");
  // =========================================================================//
  // Default Values to increase redundancy
  // =========================================================================//
  resources::initiateDefaultScript();
  resources::initiateDefaultTexture();
  // =========================================================================//
  // config.ini file
  // =========================================================================//

  return new Application(std::move(luaState), std::move(window),
                         std::move(context));
}
/* Creates window, opengl context and init glew*/
// renderer is created BEFORE the asset manager, as the asset manager retrives
// the default assets, it will slowly link some to the renderer cache
Application::Application(sol::state &&luaState, SDL_Window *window,
                         void *context)
    : m_renderer(Renderer2d::createRenderer2d()),
      m_defaultImGuiInstance(new EngineController()),
      m_luaState(std::move(luaState)), m_endGameFlags(), m_window(window),
      m_context(context)
{
  addComponentFunctions(m_luaState);
};

void Application::stopLoop(bool restartFlag)
{
  m_isGameRunning = false;
  PLOG_I("Game has been stopped on {}", fmt::ptr(this));
  m_endGameFlags.restartGame = restartFlag;
}

EndGameFlags Application::run()
{
  DeltaTime deltaTime;
  DeltaTime accumulator = 0.0;
  // double renderAccumulator = 0.0;

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
        static_cast<double>(SDL_GetPerformanceFrequency()) /
        deltaTime.getNanoSeconds();
    m_currentSample = (m_currentSample + 1) % FPS_SAMPLE_COUNT;
    if (m_currentSample % 64 == 0) { // update displayed fps
      m_defaultImGuiInstance->m_currentTPS = 0.0;
      for (const double fpsSample : m_fpsSamples) {
        m_defaultImGuiInstance->m_currentTPS += fpsSample;
      }
      m_defaultImGuiInstance->m_currentTPS /= FPS_SAMPLE_COUNT;
    }
    // =============================================================== //
    // Handle Updates
    // =============================================================== //
    {
      PROFILE_SCOPE("Application::run - Handle Updates");
      DeltaTime deltaSeconds = deltaTime * m_timeMultiplier;
      // Uncomment this to create an accumulator cap
      // accumulator = fmod(accumulator + deltaSeconds, 15.0);
      accumulator += deltaSeconds;

      // renderAccumulator += deltaSeconds;
      while (accumulator >= m_maxFrameRate) {
        m_worldScene->updateSystems(m_maxFrameRate);
        accumulator -= m_maxFrameRate;
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
        switch (event.type) {
        case SDL_QUIT:
          stopLoop();
          break;
        case SDL_WINDOWEVENT:
          if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
              event.window.windowID == SDL_GetWindowID(m_window))
            stopLoop();
          else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
            m_isMinimized = true;
          else if (event.window.event == SDL_WINDOWEVENT_RESTORED)
            m_isMinimized = false;
          else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            m_renderer.setViewport(0, 0, event.window.data1,
                                   event.window.data2);
          break;
        default:
          break;
        }
        m_worldScene->updateSystems(event);
      }
    }
    // =============================================================== //
    // Handle rendering
    // =============================================================== //
    {
      PROFILE_SCOPE("Application::run - Handle Rendering");
      // int w, h;
      // SDL_GetWindowSize(m_window, &w, &h);
      // Renderer2d::setViewport(0, 0, w, h);
      m_renderer.setClearColor(m_clearColor);
      m_renderer.clear();

      m_renderer.beginScene(lastFrameTime, *m_worldScene);
      m_worldScene->renderSystems(m_renderer, m_isMinimized, lastFrameTime);
      m_renderer.endScene();
      P_ASSERT(m_window != nullptr, "m_window is nullptr")
      SDL_GL_SwapWindow(m_window);
    }

    // =============================================================== //
    // Fix clock if it's too ahead
    // =============================================================== //
    uint64_t end = SDL_GetPerformanceCounter();
    double frameDurationSeconds =
        static_cast<double>(end - start) /
        static_cast<double>(SDL_GetPerformanceFrequency());
    if (frameDurationSeconds < m_fixedFPS) {
      SDL_Delay(
          static_cast<uint32_t>((m_fixedFPS - frameDurationSeconds) * 1000.0));
    }
  };
  PLOG_I("Reaching the end of run");
  return m_endGameFlags;
}

Application::~Application()
{
  PLOG_I("Deleting application");
  resources::clearTextures();
  resources::getDefaultLuaFile();
  SDL_GL_DeleteContext(m_context);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

} // namespace pain
