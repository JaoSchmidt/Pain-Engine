#pragma once
#include "pch.gch"

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
  // Retriever of renderers
  SDL_Renderer *getRenderer() const;
  int getMouseX();
  int getMouseY();
  // End the Cycle Pain
  void stopTheRun();
  void pushLayer(Layer *layer);
  void popLayer(Layer *layer);
  void handleEvents(const SDL_Event &event);
  void handleUpdate(DeltaTime deltaTime);
  void handleRender();
  static void glErrorHandler(unsigned int source, unsigned int type,
                             unsigned int id, unsigned int severity, int lenght,
                             const char *message, const void *userParam);

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
  LayerStack *m_layerStack;
  DeltaTime m_deltaTime = 0;
  uint64_t m_lastFrameTime = 0;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
