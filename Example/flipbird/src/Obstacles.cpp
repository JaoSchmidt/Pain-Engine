#include "Obstacles.h"

Obstacles::Obstacles(pain::Scene *scene, const glm::vec3 &position)
    : pain::GameObject(scene)
{
  addComponent<pain::TransformComponent>(position);
  addComponent<pain::MovementComponent>();
  addComponent<pain::SpritelessComponent>();
}

void ObstaclesController::onCreate()
{
  m_obstaclesSpeed = -0.5f;
  pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
  mc.m_velocityDir = glm::vec3(m_obstaclesSpeed, 0.0f, 0.0f);
}

void ObstaclesController::onUpdate(double deltaTimeSec) {}

void ObstaclesController::onEvent(const SDL_Event &e) {}

bool ObstaclesController::checkIntersection(const Player &player)
{
  pain::TransformComponent &tc = getComponent<pain::TransformComponent>();
  pain::SpritelessComponent &slc = getComponent<pain::SpritelessComponent>();

  const float maxX = std::max(tc.m_position.x, tc.m_position.x + slc.m_size.x);
  const float maxY = std::max(tc.m_position.y, tc.m_position.y + slc.m_size.y);
  const float minX = std::min(tc.m_position.x, tc.m_position.x + slc.m_size.x);
  const float minY = std::min(tc.m_position.y, tc.m_position.y + slc.m_size.y);

  const pain::TransformComponent &playerTC =
      player.getComponent<pain::TransformComponent>();
  return (playerTC.m_position.x >= minX && playerTC.m_position.x <= maxX &&
          playerTC.m_position.y >= minY && playerTC.m_position.y <= maxY);
}
