#pragma once

#include <pain.h>

class PlayerController : pain::ScriptableEntity
{
public:
  void onCreate();
  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &event);

private:
  const glm::vec3 m_acceleration = {0.f, -1.f, 0.f};
};

class Player : public pain::GameObject
{
public:
  Player(pain::Scene *scene, const glm::vec3 &position);
};
