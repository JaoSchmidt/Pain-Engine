#include "ECS/Scene.h"
#include "CoreFiles/DeltaTime.h"
#include "ECS/Registry.h"

namespace pain
{

Scene::Scene() : m_registry(new Registry()) {}

Entity Scene::createEntity()
{
  Entity id;
  if (m_availableEntities.empty()) {
    id = ++numberOfEntities;
  } else {
    id = m_availableEntities.front();
    m_availableEntities.pop();
  }
  return id;
}

void Scene::destroyEntity(Entity entity)
{
  // m_registry->removeAll(entity);
  m_registry->remove<MovementComponent>(entity);
  m_registry->remove<RotationComponent>(entity);
  m_registry->remove<TransformComponent>(entity);
  m_availableEntities.push(entity);
}

} // namespace pain
