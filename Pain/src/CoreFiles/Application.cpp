/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreFiles/Application.h"
#include "Assets/HighResolutionTimer.h"
#include "Assets/ManagerFile.h"
#include "Assets/ManagerTexture.h"
#include "ContextBackend.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreFiles/RenderPipeline.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/Profiling.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "GUI/ImGuiSys.h"
#include "Misc/BasicOrthoCamera.h"
#include "Misc/Events.h"
#include "Scripting/State.h"
#include <SDL2/SDL_version.h>
#include <memory>
#include <thread>

namespace pain
{
pain::Application *pain::Application::s_app = nullptr;

unsigned Application::getProcessorCount()
{
  return std::thread::hardware_concurrency();
}

Application *Application::createApplication(AppContext &&context,
                                            FrameBufferCreationInfo &&fbci)
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

  SDL_Window *window = SDL_CreateWindow(
      context.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      context.defaultWidth, context.defaultHeight,
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
  if (window == nullptr)
    PLOG_E("Application window not initialized");
  void *sdlContext = SDL_GL_CreateContext(window);

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
  FileManager::initiateDefaultScript();
  TextureManager::initiateDefaultTextures();
  // =========================================================================//
  // config.ini file
  // =========================================================================//

  Application *app = new Application(std::move(luaState), std::move(window),
                                     std::move(sdlContext), std::move(fbci),
                                     std::move(context));
  if (app != nullptr) {
    addComponentFunctions(app->m_luaState, app->m_worldScene);
    addScheduler(app->m_luaState, app->m_worldScene);
    app->m_worldScene.addEntityFunctions("World", app->m_luaState);
    createLuaEventMap(app->m_luaState, app->m_eventDispatcher);
    TextureManager::addRendererForDeletingTextures(&(app->m_renderer));
    Application::s_app = app;
  }
  return app;
}
/* Creates window, opengl context and init glew*/
// renderer is created BEFORE the asset manager, as the asset manager retrives
// the default assets, it will slowly link some to the renderer cache
Application::Application(sol::state &&luaState, SDL_Window *window,
                         void *sdlContext, FrameBufferCreationInfo &&fbci,
                         AppContext &&context)
    : m(), m_context(context), m_renderer(Renderer2d::createRenderer2d()),
      m_threadPool(ThreadPool{}), m_luaState(std::move(luaState)),
      m_eventDispatcher(m_luaState),
      m_worldScene(Scene::create(m_eventDispatcher, m_luaState, m_threadPool)),
      m_endGameFlags(), m_window(window), m_sdlContext(sdlContext),
      m_renderPipeline(fbci.swapChainTarget
                           ? RenderPipeline::create(m_eventDispatcher)
                           : RenderPipeline::create(fbci, m_eventDispatcher)) {
      };

void Application::stopLoop(bool restartFlag)
{
  m.isGameRunning = false;
  PLOG_I("Game has been stopped on {}", fmt::ptr(this));
  m_endGameFlags.restartGame = restartFlag;
}
EndGameFlags Application::run()
{
  // creates a dummy camera in case it doesn't exist
  if (!m_renderer.hasCamera()) {
    PLOG_W("You didn't set a camera, using the default camera");
    reg::Entity camera = Dummy2dCamera::create(
        m_worldScene, m_context.defaultWidth, m_context.defaultHeight, 1.f);
    setRendererCamera(camera, m_context.defaultWidth, m_context.defaultHeight);
  }

  // creates a dummy ui scene
  if (m_uiScene.get() == nullptr)
    createUIScene();

  // With all scenes created, we can now properly use it
  m_renderPipeline.subscribeToViewportChange(m_worldScene);

  HighResolutionTimer frameTimer;
  DeltaTime accumulator = 0.0;

  while (m.isGameRunning) {
    DeltaTime deltaTime = frameTimer.tick();
    uint64_t elapsedTime = frameTimer.elapsedNanos();

    // =============================================================== //
    // Calculate FPS sample
    // =============================================================== //

    m.fpsSamples[m.currentSample] =
        static_cast<double>(DeltaTime::oneSecond()) /
        deltaTime.getNanoSeconds();
    m.currentSample = (m.currentSample + 1) % m.FPS_SAMPLE_COUNT;

    if (m.currentSample % 64 == 0) { // update displayed fps
      double currentTPS = 0.0;
      for (const double fpsSample : m.fpsSamples) {
        currentTPS += fpsSample;
      }
      currentTPS /= m.FPS_SAMPLE_COUNT;
      IMGUI_PLOG_NAME("FPS", [currentTPS]() {
        const std::string fps = "FPS: " + std::to_string(currentTPS);
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", fps.c_str());
      });
    }

    // =============================================================== //
    // Handle Updates
    // =============================================================== //
    {
      PROFILE_SCOPE("Application::run - Handle Updates");
      DeltaTime deltaSeconds = deltaTime * m.timeMultiplier;
      accumulator += deltaSeconds;

      while (accumulator >= m.fixedFrameRate) {
        m_worldScene.updateSystems(m.fixedFrameRate);
        accumulator -= m.fixedFrameRate;
      }
    }

    // =============================================================== //
    // Handle Events
    // =============================================================== //
    {
      PROFILE_SCOPE("Application::run - Handle Events");
      SDL_Event event;
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
            m.isMinimized = true;
          else if (event.window.event == SDL_WINDOWEVENT_RESTORED)
            m.isMinimized = false;
          else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            m_renderPipeline.onWindowResized(event, m_renderer, m_worldScene);
          break;
        default:
          break;
        }
        m_worldScene.updateSystems(event);
        if (m_uiScene != nullptr)
          m_uiScene->updateSystems(event);
      }
    }

    // =============================================================== //
    // Handle rendering
    // =============================================================== //
    {
      PROFILE_SCOPE("Application::run - Handle Rendering");
      m_renderPipeline.pipeline(m_renderer, m.isMinimized, elapsedTime,
                                m_worldScene, *m_uiScene);
      P_ASSERT(m_window != nullptr, "m_window is nullptr")
      SDL_GL_SwapWindow(m_window);
    }

    // =============================================================== //
    // Frame rate limiting
    // =============================================================== //
    if (deltaTime.getSeconds() < m.fixedFPS) {
      uint32_t sleepMs =
          static_cast<uint32_t>((m.fixedFPS - deltaTime.getSeconds()) * 1000.0);
      HighResolutionTimer::sleep(sleepMs);
    }
  };

  PLOG_I("Reaching the end of run");
  return m_endGameFlags;
}

Application::~Application()
{
  PLOG_I("Deleting application");
  TextureManager::clearTextures();
  FileManager::getDefaultLuaFile();
  SDL_GL_DeleteContext(m_sdlContext);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
  s_app = nullptr;
}

} // namespace pain
