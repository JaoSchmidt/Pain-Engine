#pragma once
#include <pain.h>

#include "ImGuiController.h"
#include "Obstacles.h"
#include "Player.h"

class Game : public pain::Scene
{
public:
  Game() : pain::Scene() {}
  void onRender();
  void onCreate();
  void onUpdate(double deltaTime);
  void onEvent(const SDL_Event &event);

  float m_radians = 0.0f;
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
  std::shared_ptr<ImGuiController> m_shapeController;
  std::unique_ptr<Player> m_pplayer;
  std::shared_ptr<pain::OrthoCameraEntity> m_Camera;
  std::shared_ptr<pain::Texture> m_texture;

  constexpr static float MAX_INTERVAL = 5.0f;
  // std::array<Obstacles, NUMBER_OF_OBSTACLES> m_obstacles = {};
  void reviveObstacle();
};
