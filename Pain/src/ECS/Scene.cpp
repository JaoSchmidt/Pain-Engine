#include "ECS/Scene.h"
#include "Debugging/Profiling.h"

#include "SDL_events.h"

namespace pain
{

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

void Scene::insertStaticCollider(reg::Entity entity)
{
  // m_collisionSystem.insertStatic(
  //     entity, getComponent<TransformComponent>(entity).m_position);
}

} // namespace pain
