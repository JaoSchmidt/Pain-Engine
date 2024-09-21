#pragma once

#include <pain.h>

class PlayerController : public pain::ScriptableEntity,
                         public pain::ImGuiInstance
{
public:
  void onCreate();
  void onUpdate(double deltaTimeSec);
  void onRender(double currentTime);
  const void onImGuiUpdate() override;

  const void resetPosition();

private:
  float m_pseudoVelocityX = 1.f;
  float m_maxVelY = 2.f;
  float m_gravity = -0.9f;
  float m_jumpForce = 1.0f;
  float m_jumpImpulse = 5.f;
  float m_dampingFactor = 1.f;
  bool m_displayUpdates = false;
  // particle emission
  float m_timeSinceLastEmission = 0.f;
  float m_emissionInterval = 0.f;
};

class Player : public pain::GameObject
{
public:
  Player(pain::Scene *scene, std::shared_ptr<pain::Texture> &pTexture);
};
