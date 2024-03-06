#pragma once

#include "Core.h"
#include "LayerStack.h"
#include <SDL2/SDL.h>

namespace pain
{

class EXPORT Application
{
public:
  Application(const char *title, int x, int y, int w, int h);

  // virtual because the real Application will be the game
  virtual ~Application();
  // // The Handler of events
  // void setEventCallback(std::function<void(void)> func);
  // // The Hanlder of updates
  // void setUpdateCallback(std::function<void(void)> func);
  // // The Handler of renderers
  // void setRenderCallback(std::function<void(void)> func);
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
  void handleEvents();
  void handleUpdate();
  void handleRender();

private:
  // Refers to the game window
  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;
  bool m_isGameRunning = true;
  int m_mouseX;
  int m_mouseY;
  unsigned int m_maxFrameRate;
  // std::function<void(void)> m_eventCallback;
  // std::function<void(void)> m_updateCallback;
  // std::function<void(void)> m_renderCallback;
  LayerStack *m_layerStack;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
