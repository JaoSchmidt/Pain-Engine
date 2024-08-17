#pragma once

#include <pain.h>

class PlayerController : public pain::ScriptableEntity
{
public:
  void onCreate();
  void onUpdate(double deltaTimeSec);
  void onEvent(const SDL_Event &event);

private:
  const float m_gravity = -3.0f;
};

class Player : public pain::GameObject
{
public:
  Player(pain::Scene *scene, std::shared_ptr<pain::Texture> &pTexture);
};
