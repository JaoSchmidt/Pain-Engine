#include "Application.h"
#include "LogWrapper.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

namespace pain
{

Application::Application(const char *title, int x, int y, int w, int h)
    : m_maxFrameRate(60)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    PLOG_E("SDL could not be init", SDL_GetError());
  } else {
    PLOG_T("SDL video is initialized");
  }
  m_window = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_OPENGL);
  m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
  if (m_window != nullptr && m_renderer != nullptr) {
    PLOG_T("Application window and render initialized");
  } else {
    if (m_window == nullptr)
      PLOG_E("Application window not initialized");
    if (m_renderer == nullptr)
      PLOG_E("Renderer not initialized");
  }
  m_layerStack = new LayerStack();
}

Application::~Application()
{
  delete m_layerStack;
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void Application::pushLayer(Layer *layer) { m_layerStack->pushLayer(layer); }

void Application::popLayer(Layer *layer) { m_layerStack->popLayer(layer); }

// void Application::setEventCallback(std::function<void(void)> eventHandler)
// {
//   m_eventCallback = eventHandler;
// }
// void Application::setUpdateCallback(std::function<void(void)> updateHandler)
// {
//   m_updateCallback = updateHandler;
// }
// void Application::setRenderCallback(std::function<void(void)> renderHandler)
// {
//   m_renderCallback = renderHandler;
// }
void Application::stop() { m_isGameRunning = false; }

void Application::handleEvents()
{
  SDL_Event event;

  // (1) Handle Input
  // Start our event loop
  while (SDL_PollEvent(&event)) {
    // Handle each specific event
    if (event.type == SDL_QUIT) {
      stop();
    }
  }
}
void Application::handleUpdate()
{
  for (auto pLayer = m_layerStack->end(); pLayer != m_layerStack->begin();) {
    (*--pLayer)->onUpdate();
  }
}
void Application::handleRender()
{
  SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(m_renderer);

  // Do the actual drawing
  SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  // updates the screen
  SDL_RenderPresent(m_renderer);
}

void Application::run()
{
  while (m_isGameRunning) {
    unsigned int start = SDL_GetTicks();
    // unsigned int buttons;
    // buttons = SDL_GetMouseState(&m_mouseX, &m_mouseY);

    // handle events first
    handleEvents();
    // handle any updates
    handleUpdate();

    // hanlde our rendering
    handleRender();

    unsigned int elapsedTime = SDL_GetTicks() - start;
    if (elapsedTime < m_maxFrameRate) {
      SDL_Delay(m_maxFrameRate - elapsedTime);
    }
  };
}

} // namespace pain
