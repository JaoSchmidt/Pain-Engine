#pragma once
#include <pain.h>

#include "CelestialBodies.h"

class Game : public pain::Scene, public pain::ImGuiInstance
{
public:
  void onCreate();
  void onRender(double currenTime);
  void onUpdate(double deltaTime);
  const void onImGuiUpdate();
  void onEvent(const SDL_Event &event);
  float m_constantG = 1.f;
  int m_timeDivision = 50;

private:
  // parameters
  bool m_isRunning;

  // non-parameters
  std::unique_ptr<Star> m_bodyA;
  std::unique_ptr<Star> m_bodyB;
};
