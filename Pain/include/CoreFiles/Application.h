#pragma once
#include "ECS/SceneManager.h"
#include "pch.h"

#include "Core.h"
#include "CoreFiles/DeltaTime.h"
#include "CoreFiles/Layer.h"
#include "CoreFiles/LayerStack.h"

namespace pain
{

class EXPORT Application
{
public:
  Application(const char *title, int w, int h);

  // virtual because the real Application will be the game
  virtual ~Application();
  // Cycle of Pain
  void run();
  void stop();
  int getMouseX();
  int getMouseY();
  // End the Cycle Pain

  void stopTheRun();
  void handleEvents(const SDL_Event &event);
  void handleUpdate(DeltaTime deltaTime);
  void handleRender();

  static void glErrorHandler(unsigned int source, unsigned int type,
                             unsigned int id, unsigned int severity, int lenght,
                             const char *message, const void *userParam);
  SDL_Window *getWindow() { return m_window; }

  // ECS
  // clang-format off
  void inline pushScene(const std::string &name, Scene *scene) { m_sceneManager->addScene(name,scene); }
  void inline popScene(const std::string &name) { m_sceneManager->popScene(name); }
  void inline attachScene(const std::string &name) { m_sceneManager->attachScene(name); }
  void inline detachScene(const std::string &name) { m_sceneManager->detachScene(name); }
  // clasng-format on

private:
  void initialSetup(const char *title, int w, int h);
  // Refers to the game window
  SDL_Window *m_window = nullptr;
  SDL_GLContext m_context = nullptr;
  // SDL_Renderer *m_renderer = nullptr;
  bool m_isGameRunning = true;
  int m_mouseX;
  int m_mouseY;
  bool m_isMinimized;
  unsigned int m_maxFrameRate;
  SceneManager *m_sceneManager;
  DeltaTime m_deltaTime = 0;
  uint64_t m_lastFrameTime = 0;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
