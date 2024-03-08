#pragma once

#include "AllBuffers.h"
#include "Core.h"
#include "LayerStack.h"
#include "Shader.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <memory>

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
  void handleEvents();
  void handleUpdate();
  void handleRender();

private:
  // temp
  int m_width;
  int m_height;
  // Refers to the game window
  SDL_Window *m_window = nullptr;
  SDL_GLContext m_context = nullptr;
  // SDL_Renderer *m_renderer = nullptr;
  bool m_isGameRunning = true;
  int m_mouseX;
  int m_mouseY;
  unsigned int m_maxFrameRate;
  LayerStack *m_layerStack;
  unsigned int m_vertexArray;
  Shader *m_shader;
  VertexBuffer *m_vertexBuffer;
  IndexBuffer *m_indexBuffer;
  // std::unique_ptr<Shader> m_Shader;
  // std::unique_ptr<VertexBuffer> m_VertexBuffer;
  // std::unique_ptr<IndexBuffer> m_IndexBuffer;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
