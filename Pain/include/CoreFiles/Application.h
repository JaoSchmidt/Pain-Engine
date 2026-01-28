/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Application.h
#pragma once
#include "CoreFiles/RenderPipeline.h"
#include "ECS/UIScene.h"
#include "ECS/WorldScene.h"
#include "pch.h"

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "CoreFiles/EndGameFlags.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/DebuggingImGui.h"
#include "GUI/ImGuiSys.h"
#include <sol/state.hpp>

namespace pain
{

/**
 * @struct AppContext
 * @brief Configuration structure used to initialize the Application.
 *
 * Provides window parameters and configuration file paths used during startup.
 */
struct AppContext {
  /** Default external configuration file name. */
  static constexpr const char *configIniFile = "config.ini";

  /** Default internal configuration file name. */
  static constexpr const char *internalConfigFile = "internalConfig.ini";

  /** Window title displayed in the OS window. */
  const char *title = "Unnamed Game";

  /** Initial window width in pixels. */
  int defaultWidth = 800;

  /** Initial window height in pixels. */
  int defaultHeight = 600;
};

/**
 * @class Application
 * @brief Main engine application controller and runtime loop.
 *
 * Responsible for:
 * - SDL initialization and window creation
 * - Renderer setup
 * - Lua state management
 * - Scene creation and updates
 * - Event processing
 * - Rendering pipeline execution
 * - Frame timing and synchronization
 *
 * Only one Application instance should exist at a time.
 */
class Application
{
public:
  static Application *s_app;
  /**
   * @brief Creates and initializes a new Application instance.
   *
   * Initializes SDL, graphics context, renderer, scripting, and default assets.
   *
   * @param context Application startup configuration.
   * @param frameBufferCreationInfo Framebuffer configuration.
   * @return Pointer to the created Application.
   */
  static Application *
  createApplication(AppContext &&context,
                    FrameBufferCreationInfo &&frameBufferCreationInfo = {
                        .swapChainTarget = false});

  /** Destroys the application and releases all owned resources. */
  ~Application();

  /** Enables or disables infinite simulation speed (ignores frame limiting). */
  void setInfiniteSimulation(bool isSimulation)
  {
    m.isSimulation = isSimulation;
  };

  /** Returns the number of available hardware threads. */
  static unsigned getProcessorCount();

  /** Enable or disable the rendering. For example, if you are doing a
   * simulation or some other calculation, you might want to ingore the render
   * completly*/
  static void setRendereing(bool state = true)
  {
    P_ASSERT(s_app != nullptr,
             "`s_app` is nullptr. Did you remember to link your app to s_app?");
    s_app->m.isRendering = state;
  }
  /** Enable or disable the rendering. For example, if you are doing a
   * simulation or some other calculation, you might want to ingore the render
   * completly*/
  static void toogleRendereing()
  {
    P_ASSERT(s_app != nullptr,
             "`s_app` is nullptr. Did you remember to link your app to s_app?");
    s_app->m.isRendering = !(s_app->m.isRendering);
  }

  /** set the global simulation multiplier. Id est, _technically_ increase the
     frequency of non renderer parts. */
  static void setTimeMultiplier(double time = 1.)
  {
    P_ASSERT(s_app != nullptr,
             "`s_app` is nullptr. Did you remember to link your app to s_app?");
    s_app->m.timeMultiplier = time;
  }
  /** get the game velocity. Id est, the buff to the game update loop time
   * accumulator */
  static double getTimeMultiplier()
  {
    P_ASSERT(s_app != nullptr,
             "`s_app` is nullptr. Did you remember to link your app to s_app?");
    return s_app->m.timeMultiplier;
  }

  /** Toggle simulation */
  static void inline toggleSimulation()
  {
    P_ASSERT(s_app != nullptr,
             "`s_app` is nullptr. Did you remember to link your app to s_app?");
    s_app->m.isSimulation = !(s_app->m.isSimulation);
  }
  /** Returns a pointer to the simulation flag. */
  static bool inline isSimulation()
  {
    P_ASSERT(s_app != nullptr,
             "`s_app` is nullptr. Did you remember to link your app to s_app?");
    return s_app->m.isSimulation;
  }

  /** Returns the Lua state used by the application. */
  sol::state &getLuaState() { return m_luaState; };

  /** Returns the 2D renderer instance. */
  Renderer2d &getRenderer() { return m_renderer; }

  /** Returns the framebuffer specification used by the render pipeline. */
  const FrameBufferCreationInfo &getFrameInfo() const
  {
    return m_renderPipeline.m_frameBuffer.getSpecification();
  }

  // =============================================================== //
  // ECS / Scene Control
  // =============================================================== //

  /**
   * @brief Stops the main loop.
   *
   * @param restartFlag If true, signals the engine to restart after shutdown.
   */
  void stopLoop(bool restartFlag = false);

  /**
   * @brief Assigns the renderer camera and viewport dimensions.
   *
   * @param camera Camera entity to use.
   * @param width Viewport width in pixels.
   * @param height Viewport height in pixels.
   */
  void setRendererCamera(const reg::Entity camera, int width, int height)
  {
    m_renderer.changeCamera(camera);
    m_renderer.setViewport(0, 0, width, height);
  }

  /**
   * @brief Creates the world scene with user-defined component types.
   *
   * Automatically configures collision grid size in the renderer.
   *
   * @tparam Components Component types to attach.
   * @param collisionGridSize Grid size used for spatial partitioning.
   * @param args Component constructor arguments.
   * @return Reference to the created world Scene.
   */
  template <typename... Components>
  Scene &createWorldSceneComponents(float collisionGridSize, Components... args)
  {
    m_worldScene.createComponents(m_worldScene.getEntity(),
                                  std::forward<Components>(args)...);
    m_renderer.setCellGridSize(collisionGridSize);
    return m_worldScene;
  }

  /**
   * @brief Creates the UI scene with user-defined components.
   *
   * Automatically attaches the ImGui system.
   *
   * @tparam Components Component types to attach.
   * @param args Component constructor arguments.
   * @return Reference to the created UIScene.
   */
  template <typename... Components> UIScene &createUIScene(Components... args)
  {
    m_uiScene =
        std::make_unique<UIScene>(m_eventDispatcher, m_luaState, m_threadPool);
    m_uiScene->createComponents(m_uiScene->getEntity(),
                                std::forward<Components>(args)...);
    m_uiScene->addSystem<Systems::ImGuiSys>(m_sdlContext, m_window);
    return *m_uiScene;
  }

private:
  Application(sol::state &&luaState, SDL_Window *window, void *sdlContext,
              FrameBufferCreationInfo &&fbci, AppContext &&context);

  // =============================================================== //
  // VARIABLES / CONSTANTS
  // =============================================================== //
  struct DefaultApplicationValues {
    bool isGameRunning = true;
    bool isRendering = true;
    bool isMinimized = false;
    bool isSimulation = false;
    const double fixedUpdateTime = 1.0 / 60.0;
    const double fixedFPS = 1.0 / 60.0;
    double timeMultiplier = 1.0;
    DeltaTime fixedFrameRate = 16'666'666; /** 1/60 seconds in nanoseconds */

    /** FPS sample buffer size. */
    constexpr static int FPS_SAMPLE_COUNT = 64;
    double fpsSamples[FPS_SAMPLE_COUNT] = {0};
    int currentSample = 1;
  };

  DefaultApplicationValues m;
  const AppContext m_context;

  // =============================================================== //
  // OWNED OBJECTS
  // =============================================================== //
  std::unique_ptr<UIScene> m_uiScene = nullptr;
  Renderer2d m_renderer;
  ThreadPool m_threadPool;
  sol::state m_luaState;
  reg::EventDispatcher m_eventDispatcher;
  Scene m_worldScene;

  EndGameFlags run();
  EndGameFlags m_endGameFlags = {};

  /** Refers to the game window. */
  SDL_Window *m_window = nullptr;

  SDL_GLContext m_sdlContext = nullptr;
  RenderPipeline m_renderPipeline;

  friend struct Pain;
};

/**
 * @brief User-defined factory function implemented by the client application.
 *
 * Must return a newly created Application instance.
 */
Application *createApplication();

} // namespace pain
