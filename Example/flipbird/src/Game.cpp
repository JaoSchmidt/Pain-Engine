#include "Game.h"
#include "CameraController.h"

void Game::onCreate()
{
  const int w = 800;
  const int h = 600;
  PLOG_T("a");
  m_Camera.reset(new pain::OrthoCameraEntity(this, (float)w / h, 10.0f));
  auto &a = m_Camera->addComponent<pain::NativeScriptComponent>();
  a.bind<CameraController>();
  PLOG_T("b");
  m_pplayer = new Player(this, glm::vec3(-0.4, 0.0f, 0.0f));
  PLOG_T("c");
  // for (auto &o : m_obstacles) {
  //   o = *new Obstacles(this, glm::vec3(2.0f, 0.0f, 0.0f));
  // };
}

void Game::onUpdate(double deltaTime)
{
  if (m_isRunning) {
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
