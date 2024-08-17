#include "Player.h"
#include <cmath>

Player::Player(pain::Scene *scene, std::shared_ptr<pain::Texture> &pTexture)
    : pain::GameObject(scene)
{
  addComponent<pain::TransformComponent>(glm::vec3(-1.0f, 0.0f, 0.0f));
  addComponent<pain::MovementComponent>(glm::vec3(0.f, 0.0f, 0.0f));
  addComponent<pain::RotationComponent>();
  addComponent<pain::SpriteComponent>(
      glm::vec2(0.1f, 0.1f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, pTexture);
};

void PlayerController::onCreate() {}

void PlayerController::onUpdate(double deltaTimeSec)
{
  pain::TransformComponent &tc = getComponent<pain::TransformComponent>();
  if (tc.m_position.y <= -1.f) {
    tc.m_position.y = -1.f;
  } else {
    const float fDeltaTime = (float)deltaTimeSec;
    pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
    pain::RotationComponent &rc = getComponent<pain::RotationComponent>();
    // TODO: remove m_rotationSpeed from MovementComponent

    const float angleOfForce = cos(180.f - rc.m_rotationAngle);
    LOG_I("angle {}", angleOfForce);
    LOG_I("mc.m_rotationSpeed {}", mc.m_rotationSpeed);
    mc.m_rotationSpeed = angleOfForce * m_gravity * fDeltaTime;
    rc.m_rotationAngle = mc.m_rotationSpeed * fDeltaTime +
                         angleOfForce * fDeltaTime * fDeltaTime / 2;
    mc.m_velocityDir.y = mc.m_velocityDir.y + m_gravity * fDeltaTime;
  }
  // for (Obstacles *ob = ((Game *)m_scene)->begin();
  //      ob != ((Game *)m_scene)->end(); ++ob) {
  //   pain::NativeScriptComponent &nsc =
  //       ob->getComponent<pain::NativeScriptComponent>();
  //   const bool isInter = ((ObstaclesController &)nsc)
  //                            .checkIntersection(((Game
  //                            *)m_scene)->getPlayer());
  //   if (isInter)
  //     ((Game *)m_scene)->removeLife();
  // }
}

void PlayerController::onEvent(const SDL_Event &e)
{
  if (e.type == SDL_KEYDOWN) {
    if (e.key.keysym.sym == SDLK_SPACE) {
      pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
      pain::RotationComponent &rc = getComponent<pain::RotationComponent>();
      mc.m_velocityDir = glm::vec3(0.0f, 1.2f, 0.0f);
      mc.m_rotationSpeed = sin(150.f - rc.m_rotationAngle);
    }
  }
}
