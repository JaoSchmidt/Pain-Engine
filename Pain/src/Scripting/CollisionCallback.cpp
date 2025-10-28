#include "Scripting/CollisionCallback.h"

namespace pain
{

CollisionCallbackComponent::CollisionCallbackComponent() = default;

void CollisionCallbackComponent::clearCurrentFrame()
{
  m_previousCollisions = std::move(m_currentCollisions);
  m_currentCollisions.clear();
}

void CollisionCallbackComponent::addCurrentCollision(reg::Entity otherId)
{
  m_currentCollisions.insert(otherId);
}

bool CollisionCallbackComponent::wasCollidingLastFrame(
    reg::Entity otherId) const
{
  return m_previousCollisions.find(otherId) != m_previousCollisions.end();
}

bool CollisionCallbackComponent::isCollidingThisFrame(reg::Entity otherId) const
{
  return m_currentCollisions.find(otherId) != m_currentCollisions.end();
}

CollisionCallbackComponent::CollisionCallbackComponent(
    CollisionCallbackComponent &&other) noexcept
    : instance(other.instance),
      onCollisionEnterFunction(other.onCollisionEnterFunction),
      onCollisionStayFunction(other.onCollisionStayFunction),
      onCollisionExitFunction(other.onCollisionExitFunction),
      m_previousCollisions(std::move(other.m_previousCollisions)),
      m_currentCollisions(std::move(other.m_currentCollisions))
{
  other.instance = nullptr;
  other.onCollisionEnterFunction = nullptr;
  other.onCollisionStayFunction = nullptr;
  other.onCollisionExitFunction = nullptr;
}

CollisionCallbackComponent &CollisionCallbackComponent::operator=(
    CollisionCallbackComponent &&other) noexcept
{
  if (this != &other) {
    instance = other.instance;
    onCollisionEnterFunction = other.onCollisionEnterFunction;
    onCollisionStayFunction = other.onCollisionStayFunction;
    onCollisionExitFunction = other.onCollisionExitFunction;
    m_previousCollisions = std::move(other.m_previousCollisions);
    m_currentCollisions = std::move(other.m_currentCollisions);

    other.instance = nullptr;
    other.onCollisionEnterFunction = nullptr;
    other.onCollisionStayFunction = nullptr;
    other.onCollisionExitFunction = nullptr;
  }
  return *this;
}

} // namespace pain
