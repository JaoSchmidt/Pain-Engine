#pragma once
#include "pch.h"

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "CoreFiles/EndGameFlags.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/DebuggingImGui.h"
#include "ECS/SceneManager.h"
#include "GUI/ImGuiSystem.h"
#include <sol/state.hpp>

namespace pain
{

class Application
{
public:
  static constexpr std::string configIniFile = "config.ini";
  static Application *createApplication(const char *title, int w, int h,
                                        bool isSettingsApp = false);
  ~Application();

  // TODO: This isn't working, perhaps some Scene member needs a move
  // constructor/assignment?
  // template <typename S = Scene, typename... Args>
  // std::unique_ptr<S> createSceneUPtr(std::string name, Args &&...args)
  // {
  //   return std::make_unique<S>(name, m_context, m_window, m_luaState,
  //                              std::forward<Args>(args)...);
  // };

  // create Scene using a ptr
  template <typename S = Scene, typename... Args>
  S *createScenePtr(std::string name, Args &&...args)
  {
    return new S(name, m_context, m_window, m_luaState,
                 std::forward<Args>(args)...);
  };

  // virtual because the real Application will be the game

  // TODO: define those in the source file later
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

  // ECS
  // clang-format off
  void inline pushScene(const std::string &name, Scene *scene) { m_sceneManager->addScene(name,scene); }
  // TODO: This isn't working, perhaps some Scene member needs a move constructor/assignment?
  // void inline pushScene(const std::string &name, std::unique_ptr<Scene> scene) { m_sceneManager->addScene(name,std::move(scene)); }
  void inline popScene(const std::string &name) { m_sceneManager->popScene(name); }
  void inline attachScene(const std::string &name) { m_sceneManager->attachScene(name); }
  void inline detachScene(const std::string &name) { m_sceneManager->detachScene(name); }
  void stopLoop(bool restartFlag = false);
  // clang-format on
  void setRendererCamera(const OrthographicMatrices &cameraMatrices)
  {
    m_renderer.changeCamera(cameraMatrices);
  }

private:
  Application(sol::state &&luaState, SDL_Window *window, void *context);
  EndGameFlags run();
  EndGameFlags m_endGameFlags = {};
  // Refers to the game window
  SDL_Window *m_window = nullptr;
  SDL_GLContext m_context = nullptr;
  Renderer2d m_renderer;
  bool m_isGameRunning = true;
  bool m_isRendering = true;
  bool m_isMinimized = false;
  bool m_isSimulation = false;
  const double m_fixedUpdateTime = 1.0 / 60.0;
  const double m_fixedFPS = 1.0 / 60.0;
  double m_timeMultiplier = 1.0;
  DeltaTime m_maxFrameRate = 16'666'666; // 1/60 seconds in nanoseconds
  sol::state m_luaState;

  std::unique_ptr<SceneManager> m_sceneManager;
  EngineController *m_defaultImGuiInstance;

  // Pure Black
  // static constexpr glm::vec4 m_clearColor = glm::vec4(0.0,0.0,0.0,1);
  // Dark Grey
  static constexpr glm::vec4 m_clearColor = glm::vec4(0.2, 0.2, 0.2, 1);
  // Strong Pink
  // static constexpr glm::vec4 m_clearColor = glm::vec4(1.0, 0.2, 0.9, 1);

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
