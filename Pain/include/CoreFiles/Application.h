#pragma once
#include "CoreFiles/RenderPipeline.h"
#include "ECS/Scene.h"
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

class Application
{
public:
  static constexpr const char *configIniFile = "config.ini";
  static Application *
  createApplication(const char *title, int w, int h,
                    FrameBufferCreationInfo &&frameBufferCreationInfo);
  ~Application();

  // TODO: This is useful to mess with time e.g. set speed to as high as
  // possible
  void setInfiniteSimulation(bool isSimulation)
  {
    m_isSimulation = isSimulation;
  };
  static unsigned getProcessorCount();
  void inline disableRendering() { m_isRendering = false; }
  void inline enableRendering() { m_isRendering = true; }
  void inline setTimeMultiplier(double time) { m_timeMultiplier = time; }
  double inline *getTimeMultiplier() { return &m_timeMultiplier; }
  bool inline *getIsSimulation() { return &m_isSimulation; }
  sol::state &getLuaState() { return m_luaState; };
  Renderer2d &getRenderer() { return m_renderer; }
  const FrameBufferCreationInfo &getFrameInfo() const
  {
    return m_renderPipeline.m_frameBuffer.getSpecification();
  }

  // ECS
  void stopLoop(bool restartFlag = false);
  // clang-format on
  void setRendererCamera(const OrthographicMatrices &cameraMatrices,
                         const reg::Entity camera)
  {
    m_renderer.changeCamera(cameraMatrices, camera);
  }

  template <typename... Components>
  Scene &createScene(float collisionGridSize, Components... args)
  {
    m_worldScene = std::make_unique<Scene>(m_luaState, m_context, m_window);
    m_worldScene->createComponents(m_worldScene->getEntity(),
                                   std::forward<Components>(args)...);
    // m_worldSceneSys = std::make_unique<WorldSystems>(
    //     m_worldScene->getRegistry(), collisionGridSize, m_context, m_window);
    m_renderer.setCellGridSize(collisionGridSize);

    return *m_worldScene;
  }

private:
  Application(sol::state &&luaState, SDL_Window *window, void *context,
              RenderPipeline &&renderPipeline);
  // =============================================================== //
  // OWNED CLASSES
  // =============================================================== //
  std::unique_ptr<Scene> m_worldScene;

  Renderer2d m_renderer;

  EngineController *m_defaultImGuiInstance;

  sol::state m_luaState;

  // =============================================================== //
  // VARIABLES/CONSTANTS
  // =============================================================== //
  EndGameFlags run();
  EndGameFlags m_endGameFlags = {};
  // Refers to the game window
  SDL_Window *m_window = nullptr;
  SDL_GLContext m_context = nullptr;
  bool m_isGameRunning = true;
  bool m_isRendering = true;
  bool m_isMinimized = false;
  bool m_isSimulation = false;
  const double m_fixedUpdateTime = 1.0 / 60.0;
  const double m_fixedFPS = 1.0 / 60.0;
  double m_timeMultiplier = 1.0;
  DeltaTime m_maxFrameRate = 16'666'666; // 1/60 seconds in nanoseconds
  RenderPipeline m_renderPipeline;
  friend struct Pain;

  // FPS Calculation
  constexpr static int FPS_SAMPLE_COUNT = 64;
  double m_fpsSamples[FPS_SAMPLE_COUNT] = {0};
  int m_currentSample =
      1; // begins in 1 to loop all the way to 0 before calculation
};

// To be defined in CLIENT

Application *createApplication();
} // namespace pain
