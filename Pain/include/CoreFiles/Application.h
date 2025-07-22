#pragma once
#include "pch.h"

#include "Core.h"
#include "CoreFiles/DeltaTime.h"
#include "CoreFiles/ImGuiController.h"
#include "Debugging/DefaultImGui.h"
#include "ECS/SceneManager.h"
#include <sol/state.hpp>

namespace pain
{

class Application
{
public:
  Application(const char *title, int w, int h);
  ~Application();

  // virtual because the real Application will be the game

  // TODO: define those in the source file later
  void addImGuiInstance(ImGuiInstance *imGuiInstance)
  {
    m_imguiController->addImGuiMenu(imGuiInstance);
  }

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

  static void glErrorHandler(unsigned int source, unsigned int type,
                             unsigned int id, unsigned int severity, int lenght,
                             const char *message, const void *userParam);

  // ECS
  // clang-format off
  void inline pushScene(const std::string &name, Scene *scene) { m_sceneManager->addScene(name,scene); }
  void inline popScene(const std::string &name) { m_sceneManager->popScene(name); }
  void inline attachScene(const std::string &name) { m_sceneManager->attachScene(name); }
  void inline detachScene(const std::string &name) { m_sceneManager->detachScene(name); }
  // clasng-format on

private:
  void run();
  void stop();
  // Refers to the game window
  SDL_Window *m_window = nullptr;
  SDL_GLContext m_context = nullptr;
  bool m_isGameRunning = true;
  bool m_isRendering = true;
  bool m_isMinimized;
  bool m_isSimulation = false;
  const double m_fixedUpdateTime = 1.0 / 60.0;
  const double m_fixedFPS = 1.0 / 60.0;
  double m_timeMultiplier = 1.0;
  DeltaTime m_maxFrameRate = 16'666'666; // 1/60 seconds in nanoseconds
  sol::state m_luaState;

  SceneManager *m_sceneManager;
  EngineController *m_defaultImGuiInstance;
  std::unique_ptr<ImGuiController> m_imguiController;

  // Pure Black
  // static constexpr glm::vec4 m_clearColor = glm::vec4(0.0,0.0,0.0,1);
  // Dark Grey
  static constexpr glm::vec4 m_clearColor = glm::vec4(0.2,0.2,0.2,1);
  // Strong Pink
  //static constexpr glm::vec4 m_clearColor = glm::vec4(1.0, 0.2, 0.9, 1);

  friend struct Pain;

  // FPS Calculation
  constexpr static int FPS_SAMPLE_COUNT = 64;
  double m_fpsSamples[FPS_SAMPLE_COUNT] = {0};
  int m_currentSample = 1; // begins in 1 to loop all the way to 0 before calculation
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
