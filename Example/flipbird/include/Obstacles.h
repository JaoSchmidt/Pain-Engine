#include <pain.h>

#include "Player.h"

class ObstaclesController : pain::ScriptableEntity
{
public:
  ObstaclesController(float obstaclesSpeed);
  void revive(bool upsideDown, bool movable);

  void onCreate();
  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &event);
  void setObstaclesSpeed();
  bool checkIntersection(const Player &player);

private:
  bool m_isAlive = true;
  bool m_isUpsideDown = false;
  bool m_isMovable = true;
  float m_obstaclesSpeed = 1.0f;
  double m_deactivateTimout = 0.0f;
};

class Obstacles : public pain::GameObject
{
public:
  Obstacles(pain::Scene *scene);
  Obstacles(pain::Scene *scene, const glm::vec3 &position);
  // Obstacles &operator=(const Obstacles &o) { return *this; }
  // Obstacles(const Obstacles &o, pain::Scene *scene) : GameObject(scene) {}
};
