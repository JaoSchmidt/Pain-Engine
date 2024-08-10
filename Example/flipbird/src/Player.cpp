#include "Player.h"

Player::Player(pain::Scene *scene, const glm::vec3 &position)
    : pain::GameObject(scene)
{
  addComponent<pain::TransformComponent>(position);
  addComponent<pain::MovementComponent>();
  addComponent<pain::SpritelessComponent>(glm::vec2(0.1f, 0.1f),
                                          glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
};

void PlayerController::onCreate() {}

void PlayerController::onUpdate(double deltaTimeSec)
{
  pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
  mc.m_velocityDir = mc.m_velocityDir + m_acceleration;
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
      mc.m_velocityDir = glm::vec3(0.0f, 1.0f, 0.0f);
    }
  }
}
