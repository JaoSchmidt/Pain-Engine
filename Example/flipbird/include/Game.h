#pragma once
#include <pain.h>

#include "ImGuiController.h"
#include "Obstacles.h"
#include "Player.h"

class Game : public pain::Scene, public pain::ImGuiInstance
{
public:
  void onCreate();
  void onRender();
  void onUpdate(double deltaTime);
  const void onImGuiUpdate();
  void onEvent(const SDL_Event &event);

  void removeLife();

private:
  // parameters
  int m_numberOfObstacles = 20;
  float m_obstaclesSpacing = 0.35f;
  float m_obstaclesInterval = 1.6;
  float m_intervalTime = 0.6;
  float m_maxInterval = 1.6;
  float m_defaultObstacleSpeed = -0.32;
  float m_colorInterval = 20.;  // color waves
  float m_heightInterval = 20.; // height waves

  // non-parameters
  int m_lifes = 5;
  float m_waveColor = 90.;
  float m_waveHeight = 90.;
  bool m_isRunning = true;
  bool m_collision = false;
  int m_index = 0;

  std::shared_ptr<ImGuiController> m_shapeController;
  std::unique_ptr<Player> m_pplayer;

  std::vector<std::unique_ptr<Obstacles>> m_obstacles = {};
  void reviveObstacle(int index, float random, bool upsideDown);
  bool checkIntersection(const Player &player, const Obstacles &obstacle,
                         int index);

  template <std::size_t T>
  glm::vec2 projection(const std::array<glm::vec2, T> &shape,
                       const glm::vec2 &axis);
};
