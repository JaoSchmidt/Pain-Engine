#include "Player.h"
#include "ECS/Components/Particle.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <unistd.h>

Player::Player(pain::Scene *scene, std::shared_ptr<pain::Texture> &pTexture)
    : pain::GameObject(scene)
{
  addComponent<pain::TransformComponent>(glm::vec3(-1.0f, 0.0f, 0.0f));
  // TODO: remove m_rotationSpeed from MovementComponent
  addComponent<pain::MovementComponent>(glm::vec3(0.f, 0.0f, 0.0f), 1.0f, 0.0f);
  addComponent<pain::RotationComponent>(glm::vec3(0.f, 1.f, 0.f), 315.f);
  addComponent<pain::SpriteComponent>(
      glm::vec2(0.1f, 0.1f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, pTexture);
  addComponent<pain::ParticleSprayComponent>();
};

void PlayerController::onCreate()
{
  pain::Application::Get().addImGuiInstance((pain::ImGuiInstance *)this);

  pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
  mc.m_rotationSpeed = 0.0f;
  m_pseudoVelocityX = 1.f;
  m_gravity = -0.9f;
  m_jumpForce = 0.0f;
  m_jumpImpulse = 4.0f;
  m_dampingFactor = 50.f;
  m_maxVelY = 2.f;
  m_displayUpdates = false;
}

void PlayerController::onRender(double currentTime)
{
  // const pain::TransformComponent &tc =
  // getComponent<pain::TransformComponent>(); const pain::RotationComponent &rc
  // = getComponent<pain::RotationComponent>();

  // pain::ParticleSprayComponent &psc =
  //     getComponent<pain::ParticleSprayComponent>();
  // const Uint8 *state = SDL_GetKeyboardState(NULL);
  // if (state[SDL_SCANCODE_SPACE]) {
  //   m_jumpForce = m_jumpImpulse;
  //   // particles

  //   // Check if it's time to emit a new particle
  //   if (m_timeSinceLastEmission >= m_emissionInterval) {
  //     psc.emitParticle(currentTime, tc.m_position, rc.m_rotation);
  //     m_timeSinceLastEmission = 0.0f; // Reset the timer
  //   }
  // }
}

void PlayerController::onUpdate(double deltaTimeSec)
{
  pain::TransformComponent &tc = getComponent<pain::TransformComponent>();
  pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
  pain::RotationComponent &rc = getComponent<pain::RotationComponent>();
  pain::ParticleSprayComponent &psc =
      getComponent<pain::ParticleSprayComponent>();

  if (m_jumpForce > 0.f)
    m_jumpForce = m_jumpForce - deltaTimeSec * m_dampingFactor;
  else if (m_jumpForce <= 0.f)
    m_jumpForce = 0.f;

  m_timeSinceLastEmission += deltaTimeSec;
  psc.m_emiterPosition = rc.m_rotation * 0.2f + tc.m_position;
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_SPACE])
    m_jumpForce = m_jumpImpulse;

  float acc;
  if (tc.m_position.y > 1.f) {
    tc.m_position.y = 1.f;
    acc = m_gravity * 10.f;
  } else if (tc.m_position.y < -1.f) {
    tc.m_position.y = -1.f;
    acc = m_jumpForce * 10.f;
  } else {
    acc = m_gravity + m_jumpForce;
  }

  // velocity y
  mc.m_velocityDir.y = std::clamp(
      mc.m_velocityDir.y + acc * (float)deltaTimeSec, -m_maxVelY, m_maxVelY);
  // "velocity x"
  rc.m_rotationAngle =
      -std::numbers::pi / 2 + std::atan2(mc.m_velocityDir.y, m_pseudoVelocityX);

  if (m_displayUpdates) {
    LOG_I("---------------------------------------------");
    LOG_I("m_jumpForce {}", m_jumpForce);
    LOG_I("acc {}", acc);
    LOG_I("mc.m_velocityDir.y {}", mc.m_velocityDir.y);
    LOG_I("Y/X {}", mc.m_velocityDir.y / m_pseudoVelocityX);
    LOG_I("atan {}", rc.m_rotationAngle);
  }
}

const void PlayerController::resetPosition()
{
  pain::TransformComponent &tc = getComponent<pain::TransformComponent>();
  pain::MovementComponent &mc = getComponent<pain::MovementComponent>();
  pain::RotationComponent &rc = getComponent<pain::RotationComponent>();

  mc.m_rotationSpeed = 0.0f;
  m_pseudoVelocityX = 1.f;
  tc.m_position = {-1.f, 0.f, 0.f};
  mc.m_velocityDir = {0.f, 1.f, 0.f};
  rc.m_rotation = {0.f, 1.f, 0.f};
  rc.m_rotationAngle = 315.f;
}

const void PlayerController::onImGuiUpdate()
{
  ImGui::Begin("Player Controller");
  ImGui::Text("General Settings");
  ImGui::InputFloat("Gravity", &m_gravity, 0.01f, 1.0f, "%.3f");
  ImGui::InputFloat("Jump Impulse", &m_jumpImpulse, 0.1f, 1.0f, "%.3f");
  ImGui::InputFloat("Damping Effect", &m_dampingFactor, 0.01f, 1.0f, "%.5f");
  ImGui::InputFloat("Pseudo Velocity X", &m_pseudoVelocityX, 0.1f, 1.0f,
                    "%.3f");
  ImGui::SeparatorText("Log");
  ImGui::Checkbox("Log Updates", &m_displayUpdates);
  if (ImGui::Button("Reset Components"))
    resetPosition();
  ImGui::End();
}

// void PlayerController::onDestroy() { delete m_pIG; }
