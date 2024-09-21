#include <pain.h>

#include "Player.h"

class ObstaclesController : public pain::ScriptableEntity
{
public:
  void revive(bool upsideDown, bool movable);
  const void onImGuiUpdate();
  void setObstaclesSpeed();
  bool checkIntersection(const Player &player);
  void revive(float obstacleSpeed, float height, bool upsideDown);
  void changeColor(glm::vec3 color);

private:
  bool m_isAlive = true;
  bool m_isUpsideDown = false;
  bool m_isMovable = true;
  double m_deactivateTimout = 0.0f;
};

class Obstacles : public pain::GameObject
{
public:
  Obstacles(pain::Scene *scene);
  // Obstacles &operator=(const Obstacles &o) { return *this; }
  // Obstacles(const Obstacles &o, pain::Scene *scene) : GameObject(scene) {}
};
