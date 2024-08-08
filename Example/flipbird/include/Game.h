#include <pain.h>

#include "Obstacles.h"
#include <array>

class Game : pain::Scene
{
public:
  void onCreate();
  void onUpdate(double deltaTime);

private:
  int m_lifes = 5;
  float m_obstaclesInterval = 5.0f;
  float m_xPosLimit = -2.0f;
  bool m_isRunning = true;

  constexpr static float MAX_INTERVAL = 5.0f;
  constexpr static int NUMBER_OF_OBSTACLES = 10;
  std::array<Obstacles, NUMBER_OF_OBSTACLES> m_obstacles;

  void reviveObstacle();
};
