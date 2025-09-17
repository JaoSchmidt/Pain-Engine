#include "ECS/Scene.h"
#include "Assets/ResourceManager.h"
#include "CoreRender/System.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/Camera.h"
#include "ECS/Components/Particle.h"

#include "SDL_events.h"

namespace pain
{

// TODO: Create way to move and copy components to another scene

Entity Scene::createEntity()
{
  PROFILE_FUNCTION();
  Entity id;
  if (m_availableEntities.empty()) {
    id = ++numberOfEntities;
  } else {
    id = m_availableEntities.front();
    m_availableEntities.pop();
  }
  return id;
}

// TODO: I may need to fix this
void Scene::destroyEntity(Entity entity) { m_availableEntities.push(entity); }

void Scene::updateSystems(double deltaTime)
{
  PROFILE_FUNCTION();
  m_kinematicsSystem.onUpdate(deltaTime);
  m_nativeScriptSystem.onUpdate(deltaTime);
  m_luaSystem.onUpdate(deltaTime);
}

void Scene::updateSystems(const SDL_Event &event)
{
  PROFILE_SCOPE("Scene::updateSystems - events for nsc");
  m_nativeScriptSystem.onEvent(event);
  m_imGuiSystem.onEvent(event);
  m_luaSystem.onEvent(event);
}

} // namespace pain
