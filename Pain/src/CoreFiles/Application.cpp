#include "Application.h"
#include "LogWrapper.h"
#include "external/SDL/include/SDL3/SDL_keyboard.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

namespace pain
{

Application::Application(const char *title, int w, int h) : m_maxFrameRate(60)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    PLOG_E("SDL could not be init", SDL_GetError());
  } else {
    PLOG_T("SDL video is initialized");
  }
  m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
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

void Application::stop() { m_isGameRunning = false; }

void Application::handleEvents()
{
  SDL_Event event;

  // (1) Handle Input
  // Start our event loop
  while (SDL_PollEvent(&event)) {
    // Handle each specific event
    switch (event.type) {

    case SDL_QUIT:
      stop();
      break;
    case SDL_KEYDOWN:
      PLOG_I("key pressed: {}", SDL_GetKeyName(event.key.keysym.sym));
      break;
    default:
      break;
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
  SDL_SetRenderDrawColor(m_renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(m_renderer);

  // Do the actual drawing
  // SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

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
