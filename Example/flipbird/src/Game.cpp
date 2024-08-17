#include "Game.h"
#include "CameraController.h"
#include "Player.h"

void Game::onRender() {}
void Game::onEvent(const SDL_Event &event) {}

void Game::onCreate()
{
  const int w = 800;
  const int h = 600;
  m_Camera.reset(new pain::OrthoCameraEntity(this, (float)w / h, 1.0f));
  pain::Renderer2d::init(m_Camera);
  m_Camera->addComponent<pain::NativeScriptComponent>()
      .bind<CameraController>();

  m_texture.reset(new pain::Texture("resources/textures/Player.png"));
  m_pplayer.reset(new Player(this, m_texture));
  m_pplayer->addComponent<pain::NativeScriptComponent>()
      .bind<PlayerController>();

  // for (auto &o : m_obstacles) {
  //   o = *new Obstacles(this, glm::vec3(2.0f, 0.0f, 0.0f));
  // };
}

void Game::onUpdate(double deltaTime)
{
  if (m_isRunning) {
    m_radians = m_radians + std::fmod(10.0f * deltaTime, 2 * 3.14159265359f);
    // Overall game
    // 1. if obstacle is outside camera, call onDestroy
    // 2. check if player hits obstacles
    // 3. if hits, remove one life
    // 4. if 0 lifes, score menu

    m_obstaclesInterval -= 0.1f;
    if (m_obstaclesInterval <= 0) {
      m_obstaclesInterval = MAX_INTERVAL;
      reviveObstacle();
    }
  }
}

void Game::removeLife()
{
  m_lifes -= 1;
  if (!m_lifes)
    m_isRunning = false;
}

void Game::reviveObstacle() {}
