#pragma once

#include "glm/ext/vector_float3.hpp"
#include "glm/gtx/quaternion.hpp"
#include <pain.h>

class Star : public pain::GameObject
{
public:
  Star(pain::Scene *scene, std::shared_ptr<pain::Texture> &pTexture,
       const std::string &name, const glm::vec3 &position, float mass,
       const glm::vec3 &initialVelocity, const glm::vec3 &cor = glm::vec3(1.f));
};

class StarController : public pain::ScriptableEntity, public pain::ImGuiInstance
{
public:
  void onCreate();
  void onRender(double currenctTime);
  void onUpdate(double deltaTimeSec);
  const void onImGuiUpdate() override;
  void setInitialValues(float mass, const glm::vec3 &position,
                        const glm::vec3 &velocity);
  void setInitialValues(const std::string &name, float mass,
                        const glm::vec3 &position, const glm::vec3 &velocity);
  void resetStats();
  std::string m_name;
  void addForce(const Star *star, const StarController *starController,
                float deltaTime);

  float getMotionEnergyFromOrigin()
  {
    return m_mass * glm::length2(m_velocityDir) / 2;
  };
  float getPotentialEnergy() { return m_potentialEnergy; }
  void potentialEnergy(const StarController *otherStar);

private:
  glm::vec3 kVel(const glm::vec3 &velocity, const float deltaTime,
                 const glm::vec3 &acceleration);
  glm::vec3 kAcc(const StarController *otherStar);

  float m_mass;
  glm::vec3 m_acceleration = glm::vec3(0.f);
  glm::vec3 m_velocityDir = glm::vec3(0.f);
  glm::vec3 m_initialVelocity;
  glm::vec3 m_initialPosition;

  // energy
  float m_potentialEnergy;

  // particles
  float m_timeSinceLastEmission = 0.f;
  float m_emissionInterval = 0.02f;
};
