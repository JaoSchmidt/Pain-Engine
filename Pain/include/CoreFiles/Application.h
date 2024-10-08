#pragma once
#include "pch.h"

#include "Core.h"
#include "CoreFiles/DeltaTime.h"
#include "CoreFiles/ImGuiController.h"
#include "ECS/SceneManager.h"

namespace pain
{

class EXPORT Application
{
public:
  Application(const char *title, int w, int h);

  // virtual because the real Application will be the game
  virtual ~Application();
  static Application &Get() { return *s_instance; }

  // TODO: define those in the source file later
  void addImGuiInstance(ImGuiInstance *imGuiInstance)
  {
    m_imguiController->addImGuiMenu(imGuiInstance);
  }

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
  bool m_isMinimized;
  DeltaTime m_maxFrameRate = 16'666'666; // 1/60 seconds in nanoseconds

  SceneManager *m_sceneManager;
  std::unique_ptr<ImGuiController> m_imguiController;

  // Pure Black
  static constexpr glm::vec4 m_clearColor = glm::vec4(0.0,0.0,0.0,1);
  // Dark Grey
  // static constexpr glm::vec4 m_clearColor = glm::vec4(0.2,0.2,0.2,1);
  // Strong Pink
  //static constexpr glm::vec4 m_clearColor = glm::vec4(1.0, 0.2, 0.9, 1);

  friend struct Pain;
  static Application* s_instance;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
