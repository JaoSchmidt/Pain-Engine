#include "Game.h"

void Game::onCreate()
{
  for (auto &o : m_obstacles) {
    o = *new Obstacles(this);
  };
}

void Game::onUpdate(double deltaTime)
{
  // check if player hits obstacles
  // if hits, remove one life
  // if 0 lifes, score menu
  // if obstacle is outside camera, call onDestroy
  if (m_isRunning) {
    m_obstaclesInterval -= 0.1f;
    if (m_obstaclesInterval <= 0) {
      m_obstaclesInterval = MAX_INTERVAL;
      reviveObstacle();
    }

    if ()
  }
}
