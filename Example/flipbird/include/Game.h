#pragma once
#include <pain.h>

#include "Obstacles.h"
#include "Player.h"
#include <array>

class Game : public pain::Scene
{
public:
  Game() : pain::Scene() {}
  void onCreate();
  void onUpdate(double deltaTime);
  void onEvent(const SDL_Event &event){};

  constexpr static int NUMBER_OF_OBSTACLES = 10;
  // std::array<Obstacles, NUMBER_OF_OBSTACLES>::iterator begin()
  // {
  //   return m_obstacles.begin();
  // }
  // std::array<Obstacles, NUMBER_OF_OBSTACLES>::iterator end()
  // {
  //   return m_obstacles.end();
  // }
  const Player &getPlayer() const { return *m_pplayer; }

  void removeLife();

private:
  int m_lifes = 5;
  float m_obstaclesInterval = 5.0f;
  float m_xPosLimit = -2.0f;
  bool m_isRunning = true;
  Player *m_pplayer;
  std::unique_ptr<pain::OrthoCameraEntity> m_Camera;

  constexpr static float MAX_INTERVAL = 5.0f;
  // std::array<Obstacles, NUMBER_OF_OBSTACLES> m_obstacles = {};

  void reviveObstacle();
};
