#include "Game.h"
#include "CelestialBodies.h"
#include "imgui.h"
#include <memory>

void Game::onRender(double currentTime) {}
void Game::onEvent(const SDL_Event &event) {}

void Game::onCreate()
{
  const int w = 800;
  const int h = 600;

  std::shared_ptr<pain::OrthoCameraEntity> camera =
      std::make_shared<pain::OrthoCameraEntity>(this, (float)w / h, 3.0f);
  pain::Renderer2d::init(camera);
  camera->addComponent<pain::NativeScriptComponent>()
      .bind<pain::OrthoCameraController>();

  auto texture = std::make_shared<pain::Texture>("resources/textures/star.png");
  m_bodyA = std::make_unique<Star>( //
      this, texture,                //
      "Blue", glm::vec3(0.6f, 0.f, 0.f), 2.f, glm::vec3(0.f, -1.f, 0.f),
      glm::vec3(0.1f, 1.f, 1.f));
  m_bodyB = std::make_unique<Star>( //
      this, texture,                //
      "Red", glm::vec3(-0.6f, 0.f, 0.f), 2.f, glm::vec3(0.f, 1.f, 0.f),
      glm::vec3(1.0f, 0.1f, 0.1f));

  m_isRunning = true;

  pain::Application::Get().addImGuiInstance((ImGuiInstance *)this);
}

const void Game::onImGuiUpdate()
{
  StarController *controllerA =
      ((StarController *)m_bodyA->getComponent<pain::NativeScriptComponent>()
           .instance);
  StarController *controllerB =
      ((StarController *)m_bodyB->getComponent<pain::NativeScriptComponent>()
           .instance);
  const float potentialEnergy = controllerA->getPotentialEnergy();

  ImGui::SeparatorText("System Controller");
  ImGui::InputFloat("Consant G", &m_constantG);
  ImGui::Text("Energy = m1*v1²/2 + m2*v2²/2 - G(m1m2)/r");
  ImGui::Text("%f = %f + %f - %f",
              controllerA->getMotionEnergyFromOrigin() +
                  controllerB->getMotionEnergyFromOrigin() - potentialEnergy,
              controllerA->getMotionEnergyFromOrigin(),
              controllerB->getMotionEnergyFromOrigin(), potentialEnergy);
  if (ImGui::Button("Reset System")) {
    controllerA->resetStats();
    controllerB->resetStats();
  }
  ImGui::SeparatorText("Interesting Staring Parameters");
  if (ImGui::Button("OperaGX logo")) {
    controllerA->setInitialValues(2.f, glm::vec3(0.6f, 0.f, 0.f),
                                  glm::vec3(0.f, -1.f, 0.f));
    controllerA->resetStats();
    controllerB->setInitialValues(2.f, glm::vec3(-0.6f, 0.f, 0.f),
                                  glm::vec3(0.f, 1.f, 0.f));
    controllerB->resetStats();
  }
  if (ImGui::Button("Cartoon eye")) {
    controllerA->setInitialValues(4.f, glm::vec3(0.6f, 0.f, 0.f),
                                  glm::vec3(0.f, -2.f, 0.f));
    controllerA->resetStats();
    controllerB->setInitialValues(8.f, glm::vec3(-0.6f, 0.f, 0.f),
                                  glm::vec3(0.f, 1.f, 0.f));
    controllerB->resetStats();
  }
  if (ImGui::Button("Sun earth")) {
    controllerA->setInitialValues(4.f, glm::vec3(0.6f, 0.f, 0.f),
                                  glm::vec3(0.f, -10.f, 0.f));
    controllerA->resetStats();
    controllerB->setInitialValues(6000.f, glm::vec3(-20.f, 0.f, 0.f),
                                  glm::vec3(0.f, 0.f, 0.f));
    controllerB->resetStats();
  }
  if (ImGui::Button("Elliptic orbits")) {
    controllerA->setInitialValues(2.f, glm::vec3(0.6f, 0.f, 0.f),
                                  glm::vec3(0.f, -1.5f, 0.f));
    controllerA->resetStats();
    controllerB->setInitialValues(3.f, glm::vec3(-0.6f, 0.f, 0.f),
                                  glm::vec3(0.f, 1.f, 0.f));
    controllerB->resetStats();
  }
}

void Game::onUpdate(double deltaTime)
{

  StarController *controllerA =
      ((StarController *)m_bodyA->getComponent<pain::NativeScriptComponent>()
           .instance);
  StarController *controllerB =
      ((StarController *)m_bodyB->getComponent<pain::NativeScriptComponent>()
           .instance);

  for (int i = 0; i < m_timeDivision; i++) {
    controllerA->addForce(m_bodyB.get(), controllerB,
                          (deltaTime / m_timeDivision));
    controllerB->addForce(m_bodyA.get(), controllerA,
                          (deltaTime / m_timeDivision));
  }
  // controllerA->addForce(m_bodyB.get(), controllerB, deltaTime);
  // controllerB->addForce(m_bodyA.get(), controllerA, deltaTime);
}
