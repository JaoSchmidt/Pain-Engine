#include "ECS/Scene.h"
#include "CoreFiles/DeltaTime.h"

namespace pain
{
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
  m_registry.removeAll(entity);
  m_availableEntities.push(entity);
}

void Scene::onUpdate(DeltaTime dt)
{
  MovementSystem::update(getComponentMap<MovementComponent>(),
                         getComponentMap<RotationComponent>(), dt);
}

} // namespace pain
