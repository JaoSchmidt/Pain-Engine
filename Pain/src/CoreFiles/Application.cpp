/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Application.cpp
#include "CoreFiles/Application.h"
#include "Assets/HighResolutionTimer.h"
#include "Assets/ManagerFile.h"
#include "Assets/ManagerTexture.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreFiles/RenderPipeline.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/Profiling.h"
#include "GUI/ImGuiDebugRegistry.h"
#include "GUI/ImGuiSys.h"
#include "Misc/BasicOrthoCamera.h"
#include "Misc/Events.h"
#include "Scripting/Lua/State.h"
#include "platform/ContextBackend.h"
#include <SDL2/SDL_version.h>
#include <memory>
#include <thread>

namespace pain
{
Application *Application::createApplication(AppInit &&context,
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
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_Window *window = SDL_CreateWindow(
      context.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      context.defaultWidth, context.defaultHeight,
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
  if (window == nullptr)
    PLOG_E("Application window not initialized");

  SDL_version sdl_version;
  SDL_GetVersion(&sdl_version);
  PLOG_T("SDL version: {}.{}.{}", sdl_version.major, sdl_version.minor,
         sdl_version.patch);

  void *sdlContext = SDL_GL_CreateContext(window);
  backend::Init();
  // =========================================================================//
  // Application Initial setup before
  // =========================================================================//

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

  Application *app = new Application(std::move(window), std::move(sdlContext),
                                     std::move(fbci), std::move(context));
  if (app != nullptr) {
    addComponentFunctions(app->m_ctx.luaState, app->m_runtime.worldScene);
    addScheduler(app->m_ctx.luaState, app->m_runtime.worldScene);
    app->m_runtime.worldScene.addEntityFunctions("World", app->m_ctx.luaState);
    createLuaEventMap(app->m_ctx.luaState, app->m_ctx.eventDispatcher);
    TextureManager::addRendererForDeletingTextures(app->m_ctx.renderers);
  }
  return app;
}
UIScene &Application::createUIScene()
{
  m_runtime.uiScene = std::make_unique<UIScene>(
      m_ctx.eventDispatcher, m_ctx.luaState, m_ctx.threadPool);
  m_runtime.uiScene->addSystem<Systems::ImGuiSys>(m_ctx.sdlContext,
                                                  m_ctx.window);
  return *m_runtime.uiScene;
}

EngineContext::EngineContext(SDL_Window *window, void *sdlContext,
                             FrameBufferCreationInfo &&fbci)
    :                                 //
      threadPool(ThreadPool{}),       //
      luaState(createLuaState()),     //
      eventDispatcher(luaState),      //
      renderers(Renderers::create()), //
      renderPipeline(fbci.swapChainTarget
                         ? RenderPipeline::create(eventDispatcher)
                         : RenderPipeline::create(fbci, eventDispatcher)), //
      window(window),                                                      //
      sdlContext(sdlContext)                                               //
{};

/** Initiate the application in the following order:
 * m_config for small configurations
 * m_ctx for the engine context. Must work relatively independent
 * m_runtime for the world scene, which needs the engine context working
 */
Application::Application(SDL_Window *window, void *sdlContext,
                         FrameBufferCreationInfo &&fbci, AppInit &&context)
    : m_config{.init = context}, m_ctx(window, sdlContext, std::move(fbci)),
      m_runtime{.worldScene = Scene::create(m_ctx.eventDispatcher,
                                            m_ctx.luaState, m_ctx.threadPool)},
      m_endGameFlags() {};

void Application::stopLoop(bool restartFlag)
{
  m_config.isGameRunning = false;
  PLOG_I("Game has been stopped on {}", fmt::ptr(this));
  m_endGameFlags.restartGame = restartFlag;
}

void Application::ensureCamera()
{
  // set some camera
  if (!m_ctx.renderers.m_renderer2d.hasCamera() &&
      !m_ctx.renderers.m_renderer3d.hasCamera()) {
    PLOG_I("Camera is missing, searching for 2d camera component");
    bool hasCameraComponent = false;
    for (auto &chunk : m_runtime.worldScene.query<cmp::OrthoCamera>()) {
      auto *c = std::get<0>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; i++) {
        hasCameraComponent = true;
        set2dRendererCamera(c[i].m_entity, m_config.init.defaultWidth,
                            m_config.init.defaultHeight);
      }
    }
    for (auto &chunk : m_runtime.worldScene.query<cmp::PerspCamera>()) {
      auto *c = std::get<0>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; i++) {
        hasCameraComponent = true;
        set3dRendererCamera(c[i].m_entity, m_config.init.defaultWidth,
                            m_config.init.defaultHeight);
      }
    }

    // creates a dummy camera in case it doesn't exist
    if (!hasCameraComponent) {
      PLOG_W("You didn't set a camera, using a default camera");
      reg::Entity camera = Dummy2dCamera::create(
          m_runtime.worldScene, m_config.init.defaultWidth,
          m_config.init.defaultHeight, 1.f);
      set2dRendererCamera(camera, m_config.init.defaultWidth,
                          m_config.init.defaultHeight);
    }
  }
}

EndGameFlags Application::run()
{
  backend::InitRenderer();
  ensureCamera();
  // creates a dummy ui scene
  if (m_runtime.uiScene.get() == nullptr)
    createUIScene();

  // With all scenes created, we can now properly use it
  m_ctx.renderPipeline.subscribeToEvents(m_runtime.worldScene, m_ctx.renderers);

  HighResolutionTimer frameTimer;
  DeltaTime accumulator = 0.0;

  while (m_config.isGameRunning) {
    DeltaTime deltaTime = frameTimer.tick();
    uint64_t elapsedTime = frameTimer.elapsedNanos();

    // =============================================================== //
    // Calculate FPS sample
    // =============================================================== //

    m_config.fpsSamples[m_config.currentSample] =
        static_cast<double>(DeltaTime::oneSecond()) /
        deltaTime.getNanoSeconds();
    m_config.currentSample =
        (m_config.currentSample + 1) % m_config.FPS_SAMPLE_COUNT;

    if (m_config.currentSample % 64 == 0) { // update displayed fps
      double currentTPS = 0.0;
      for (const double fpsSample : m_config.fpsSamples) {
        currentTPS += fpsSample;
      }
      currentTPS /= m_config.FPS_SAMPLE_COUNT;
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
      DeltaTime deltaSeconds = deltaTime * m_config.timeMultiplier;
      accumulator += deltaSeconds;

      while (accumulator >= m_config.fixedFrameRate) {
        m_runtime.worldScene.updateSystems(m_config.fixedFrameRate);
        accumulator -= m_config.fixedFrameRate;
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
              event.window.windowID == SDL_GetWindowID(m_ctx.window))
            stopLoop();
          else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
            m_config.isMinimized = true;
          else if (event.window.event == SDL_WINDOWEVENT_RESTORED)
            m_config.isMinimized = false;
          else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            m_ctx.renderPipeline.onWindowResized(event, m_ctx.renderers,
                                                 m_runtime.worldScene);
          break;
        default:
          break;
        }
        m_runtime.worldScene.updateSystems(event);
        if (m_runtime.uiScene != nullptr)
          m_runtime.uiScene->updateSystems(event);
      }
    }

    // =============================================================== //
    // Handle rendering
    // =============================================================== //
    {
      PROFILE_SCOPE("Application::run - Handle Rendering");
      m_ctx.renderPipeline.pipeline(m_ctx.renderers, m_config.isMinimized,
                                    elapsedTime, m_runtime.worldScene,
                                    *m_runtime.uiScene);
      P_ASSERT(m_ctx.window != nullptr, "m_window is nullptr")
      SDL_GL_SwapWindow(m_ctx.window);
    }

    // =============================================================== //
    // Frame rate limiting
    // =============================================================== //
    if (deltaTime.getSeconds() < m_config.fixedFPS) {
      uint32_t sleepMs = static_cast<uint32_t>(
          (m_config.fixedFPS - deltaTime.getSeconds()) * 1000.0);
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
  SDL_GL_DeleteContext(m_ctx.sdlContext);
  SDL_DestroyWindow(m_ctx.window);
  SDL_Quit();
}

} // namespace pain
