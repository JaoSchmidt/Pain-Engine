#include "Obstacles.h"

Obstacles::Obstacles(pain::Scene *scene) : pain::GameObject(scene)
{
  addComponent<pain::TransformComponent>(glm::vec3(2.0f, -0.5f, 0.f));
  addComponent<pain::MovementComponent>();
  addComponent<pain::TrianguleComponent>(glm::vec2(0.8f, 2.00f),
                                         glm::vec4(0.2f, 0.3f, 0.9f, 1.0f));
}

void ObstaclesController::changeColor(glm::vec3 color)
{
  pain::TrianguleComponent &tgc = getComponent<pain::TrianguleComponent>();
  tgc.m_color = {color, 1.0f};
}

void ObstaclesController::revive(float obstacleSpeed, float height,
                                 bool upsideDown)
{
  pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
  pain::TransformComponent &tc = getComponent<pain::TransformComponent>();
  pain::TrianguleComponent &tgc = getComponent<pain::TrianguleComponent>();
  tgc.m_color = {0.5f, 0.5f, 0.5f, 1.0f};

  if (upsideDown)
    tgc.m_height = {0.8f, -2.f};
  else
    tgc.m_height = {0.8f, 2.f};

  mc.m_velocityDir.x = obstacleSpeed;
  // WARN: This value "1.5f" to put all obstacles hidden on the right of the
  // screen might not work depending on the resolution. Consider alternatives
  tc.m_position = glm::vec3(1.5f, height, 0.f);
}

bool ObstaclesController::checkIntersection(const Player &player)
{
  // pain::TransformComponent &tc = getComponent<pain::TransformComponent>();
  // pain::TrianguleComponent &tgc = getComponent<pain::TrianguleComponent>();

  // const float maxX = std::max(tc.m_position.x, tc.m_position.x +
  // tgc.m_size.x); const float maxY = std::max(tc.m_position.y, tc.m_position.y
  // + tgc.m_size.y); const float minX = std::min(tc.m_position.x,
  // tc.m_position.x + tgc.m_size.x); const float minY =
  // std::min(tc.m_position.y, tc.m_position.y + tgc.m_size.y);

  // const pain::TransformComponent &playerTC =
  //     player.getComponent<pain::TransformComponent>();
  // return (playerTC.m_position.x >= minX && playerTC.m_position.x <= maxX &&
  //         playerTC.m_position.y >= minY && playerTC.m_position.y <= maxY);
  return false;
}
