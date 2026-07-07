/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Application.h
#pragma once
#include "CoreFiles/AppInitConfig.h"
#include "CoreFiles/RenderPipeline.h"
#include "CoreRender/Renderer/Renderers.h"
#include "ECS/UIScene.h"
#include "ECS/WorldScene.h"
#include "pch.h"

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "CoreFiles/EndGameFlags.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include <sol/state.hpp>

namespace pain
{
struct EngineContext {
  /// Multi Thread Pool
  ThreadPool threadPool;
  /// Owns the lua virtual machine and "sol" stuff
  sol::state luaState;
  /// Event bus manager
  reg::EventDispatcher eventDispatcher;
  /// Default owner of render passes and material/shader systems
  Renderers renderers;
  /// Mostly render Pipeline for the "renderers" behaviour
  RenderPipeline renderPipeline;
  /// Refers to the game window.
  SDL_Window *window = nullptr;
  SDL_GLContext sdlContext = nullptr;
  EngineContext(SDL_Window *window, void *sdlContext,
                const FrameBufferCreationInfo &fbci);
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
  createApplication(AppInit &&initConfig,
                    const FrameBufferCreationInfo &frameBufferCreationInfo = {
                        .swapChainTarget = false});

  NONCOPYABLE(Application);
  NONMOVABLE(Application);

  /** Destroys the application and releases all owned resources. */
  ~Application();

  /** Enables or disables infinite simulation speed (ignores frame limiting). */
  void setInfiniteSimulation(bool isSimulation)
  {
    m_config.isSimulation = isSimulation;
  };

  /** Enable or disable the rendering. For example, if you are doing a
   * simulation or some other calculation, you might want to ingore the render
   * completly*/
  void setRendereing(bool state = true) { m_config.isRendering = state; }
  /** Enable or disable the rendering. For example, if you are doing a
   * simulation or some other calculation, you might want to ingore the render
   * completly*/
  void toogleRendereing() { m_config.isRendering = !(m_config.isRendering); }

  /** set the global simulation multiplier. Id est, _technically_ increase the
     frequency of non renderer parts. */
  void setTimeMultiplier(double time = 1.) { m_config.timeMultiplier = time; }
  /** get the game velocity. Id est, the buff to the game update loop time
   * accumulator */
  double getTimeMultiplier() const { return m_config.timeMultiplier; }

  /** Disable viewport events */
  void inline setFocusedOrHovered(bool isFocusedOrHovered)
  {
    m_config.isFocusedOrHovered = isFocusedOrHovered;
  }
  /** Toggle simulation */
  void inline toggleSimulation()
  {
    m_config.isSimulation = !(m_config.isSimulation);
  }
  /** Returns a pointer to the simulation flag. */
  bool inline isSimulation() const { return m_config.isSimulation; }

  /** Returns the Lua state used by the application. */
  sol::state &getLuaState() { return m_ctx.luaState; };

  /** Returns the 2D renderer instance. */
  Renderers &getRenderers() { return m_ctx.renderers; }

  /** Returns the 2D renderer instance. */
  void *getRenderContext() const { return m_ctx.sdlContext; }
  SDL_Window *getRenderWindow() const { return m_ctx.window; }

  /** Returns the framebuffer specification used by the render pipeline. */
  const FrameBufferCreationInfo &getFrameInfo() const
  {
    return m_ctx.renderPipeline.m_frameBuffer.getSpecification();
  }

  /** Returns the current config */
  const AppInit &getCurrentConfig() const { return m_config.init; }

  // =============================================================== //
  // ECS / Scene Control
  // =============================================================== //

  /**
   * @brief Stops the main loop.
   *
   * @param restartFlag If true, signals the engine to restart after shutdown.
   */
  void stopLoop(bool restartFlag = false);

  /// @brief Assigns the renderer camera and viewport dimensions.
  void set2dRendererCamera(const reg::Entity cameraEntity, int width = 0,
                           int height = 0)
  {
    m_ctx.renderers.m_renderer2d.changeCamera(cameraEntity);
    if (width != 0 || height != 0) {
      m_ctx.renderers.setViewPort(0, 0, width, height);
    }
  }
  /// @brief Assigns the renderer camera and viewport dimensions.
  void set3dRendererCamera(const reg::Entity cameraEntity, int width = 0,
                           int height = 0)
  {
    m_ctx.renderers.m_renderer3d.changeCamera(cameraEntity);
    if (width != 0 || height != 0)
      m_ctx.renderers.setViewPort(0, 0, width, height);
  }

  /**
   * @brief Reference to the created world Scene.
   */
  Scene &getWorldScene() { return m_runtime.worldScene; }

  /**
   * @brief Creates the UI scene with user-defined components.
   *
   * Automatically attaches the ImGui system.
   *
   * @return Reference to the created UIScene.
   */
  UIScene &createUIScene();

private:
  Application(SDL_Window *window, void *sdlContext,
              const FrameBufferCreationInfo &fbci, AppInit initConfig);

  void ensureCamera();

  // =============================================================== //
  // VARIABLES / CONSTANTS
  // =============================================================== //
  struct DefaultApplicationValues {
    bool isGameRunning = true;
    bool isRendering = true;
    bool isMinimized = false;
    bool isSimulation = false;
    bool isFocusedOrHovered = true;
    constexpr static double fixedUpdateTime = 1.0 / 60.0;
    constexpr static double fixedFPS = 1.0 / 60.0;
    double timeMultiplier = 1.0;
    DeltaTime fixedFrameRate = 16'666'666; /** 1/60 seconds in nanoseconds */

    /** FPS sample buffer size. */
    constexpr static int FPS_SAMPLE_COUNT = 64;
    double fpsSamples[FPS_SAMPLE_COUNT] = {0};
    int currentSample = 1;
    double currentTPS = 0;
    AppInit init;
  } m_config;

  // =============================================================== //
  // OWNED OBJECTS
  // =============================================================== //

  EngineContext m_ctx;

  struct Runtime {
    Scene worldScene;
    // optional scenes
    std::unique_ptr<UIScene> uiScene = nullptr;
  } m_runtime;

  EndGameFlags run();
  EndGameFlags m_endGameFlags = {};

  friend struct Pain;
};

/**
 * @brief User-defined factory function implemented by the client application.
 *
 * Must return a newly created Application instance.
 */
Application *createApplication();

} // namespace pain
