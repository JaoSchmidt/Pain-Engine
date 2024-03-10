#pragma once
#include "pch.gch"

#include "AllBuffers.h"
#include "Camera.h"
#include "Core.h"
#include "Layer.h"
#include "LayerStack.h"
#include "Shader.h"
#include "VertexArray.h"

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
  unsigned int m_maxFrameRate;
  LayerStack *m_layerStack;
  std::shared_ptr<VertexArray> m_vertexArray;
  std::shared_ptr<Shader> m_shader;
  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  std::shared_ptr<IndexBuffer> m_indexBuffer;
  std::shared_ptr<Camera> m_orthocamera;
};

// To be defined in CLIENT
Application *CreateApplication();

} // namespace pain
