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
#include "profile.h"
#include <sol/state.hpp>

template <class F> class with_result_of_t
{
  F &&fun;

public:
  using T = decltype(std::declval<F &&>()());
  explicit with_result_of_t(F &&f) : fun(std::forward<F>(f)) {}
  operator T() { return fun(); }
};

template <class F> inline with_result_of_t<F> with_result_of(F &&f)
{
  return with_result_of_t<F>(std::forward<F>(f));
}

namespace pain
{
struct AppContext {
  static constexpr const char *configIniFile = "config.ini";
  static constexpr const char *internalConfigFile = "internalConfig.ini";
  const char *title;
  int defaultWidth = 800;
  int defaultHeight = 600;
};

class Application
{
public:
  static Application *
  createApplication(AppContext &&context,
                    FrameBufferCreationInfo &&frameBufferCreationInfo = {
                        .swapChainTarget = false});
  ~Application();

  // TODO: This is useful to mess with time e.g. set speed to as high as
  // possible
  void setInfiniteSimulation(bool isSimulation)
  {
    m.isSimulation = isSimulation;
  };
  static unsigned getProcessorCount();
  void inline disableRendering() { m.isRendering = false; }
  void inline enableRendering() { m.isRendering = true; }
  void inline setTimeMultiplier(double time) { m.timeMultiplier = time; }
  double inline *getTimeMultiplier() { return &m.timeMultiplier; }
  bool inline *getIsSimulation() { return &m.isSimulation; }
  sol::state &getLuaState() { return m_luaState; };
  Renderer2d &getRenderer() { return m_renderer; }
  const FrameBufferCreationInfo &getFrameInfo() const
  {
    return m_renderPipeline.m_frameBuffer.getSpecification();
  }

  // ECS
  void stopLoop(bool restartFlag = false);
  // clang-format on
  void setRendererCamera(const reg::Entity camera)
  {
    m_renderer.changeCamera(camera);
  }

  template <typename... Components>
  Scene &createWorldSceneComponents(float collisionGridSize, Components... args)
  {
    m_worldScene.createComponents(m_worldScene.getEntity(),
                                  std::forward<Components>(args)...);
    // m_worldSceneSys = std::make_unique<WorldSystems>(
    //     m_worldScene->getRegistry(), collisionGridSize, m_context, m_window);
    m_renderer.setCellGridSize(collisionGridSize);
    return m_worldScene;
  }
  template <typename... Components> UIScene &createUIScene(Components... args)
  {
    m_uiScene =
        std::make_unique<UIScene>(m_eventDispatcher, m_luaState, m_threadPool);
    m_uiScene->createComponents(m_uiScene->getEntity(),
                                std::forward<Components>(args)...);
    m_uiScene->addSystem<Systems::ImGuiSys>(m_sdlContext, m_window);
    // m_worldSceneSys = std::make_unique<WorldSystems>(
    //     m_worldScene->getRegistry(), collisionGridSize, m_context, m_window);

    return *m_uiScene;
  }

private:
  Application(sol::state &&luaState, SDL_Window *window, void *sdlContext,
              FrameBufferCreationInfo &&fbci, AppContext &&context);
  // =============================================================== //
  // VARIABLES/CONSTANTS
  // =============================================================== //
  struct DefaultApplicationValues {
    bool isGameRunning = true;
    bool isRendering = true;
    bool isMinimized = false;
    bool isSimulation = false;
    const double fixedUpdateTime = 1.0 / 60.0;
    const double fixedFPS = 1.0 / 60.0;
    double timeMultiplier = 1.0;
    DeltaTime maxFrameRate = 16'666'666; // 1/60 seconds in nanoseconds

    // FPS Calculation
    constexpr static int FPS_SAMPLE_COUNT = 64;
    double fpsSamples[FPS_SAMPLE_COUNT] = {0};
    int currentSample =
        1; // begins in 1 to loop all the way to 0 before calculation
  };
  DefaultApplicationValues m;
  const AppContext m_context;
  // =============================================================== //
  // OWNED CLASSES
  // =============================================================== //
  std::unique_ptr<UIScene> m_uiScene = nullptr;

  Renderer2d m_renderer;

  EngineController *m_defaultImGuiInstance;
  ThreadPool m_threadPool;
  sol::state m_luaState;
  reg::EventDispatcher m_eventDispatcher;
  Scene m_worldScene;

  EndGameFlags run();
  EndGameFlags m_endGameFlags = {};
  // Refers to the game window
  SDL_Window *m_window = nullptr;
  SDL_GLContext m_sdlContext = nullptr;

  RenderPipeline m_renderPipeline;
  friend struct Pain;
};

// To be defined in CLIENT

Application *createApplication();
} // namespace pain
