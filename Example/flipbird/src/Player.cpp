#include "Player.h"

Player::Player(pain::Scene *scene) : pain::GameObject(scene)
{
  addComponent<pain::TransformComponent>();
  addComponent<pain::MovementComponent>();
  addComponent<pain::SpritelessComponent>();
};

void PlayerController::onUpdate(double deltaTimeSec) {}
