#include "ECS/WorldSys.h"

namespace pain
{
WorldSystems::WorldSystems(reg::ArcheRegistry<ComponentManager> &registry,
                           float collisionGridSize, void *context,
                           SDL_Window *window)

    : m_registry{registry},                       //
      m_renderSystem{m_registry},                 //
      m_kinematicsSystem{m_registry},             //
      m_nativeScriptSystem{m_registry},           //
      m_imGuiSystem{m_registry, context, window}, //
      m_luaSystem{m_registry},                    //
      m_collisionSystem{collisionGridSize, m_registry} {};

void WorldSystems::updateSystems(double deltaTime)
{
  PROFILE_FUNCTION();
  m_kinematicsSystem.onUpdate(deltaTime);
  m_nativeScriptSystem.onUpdate(deltaTime);
  m_luaSystem.onUpdate(deltaTime);
}

void WorldSystems::updateSystems(const SDL_Event &event)
{
  PROFILE_SCOPE("WorldSystems::updateSystems - events for nsc");
  m_nativeScriptSystem.onEvent(event);
  m_imGuiSystem.onEvent(event);
  m_luaSystem.onEvent(event);
}
//
// void WorldSystems::insertStaticCollider(reg::Entity entity)
// {
//   // m_collisionSystem.insertStatic(
//   //     entity, getComponent<TransformComponent>(entity).m_position);
// }

void WorldSystems::renderSystems(Renderer2d &renderer, bool isMinimized,
                                 double currentTime)
{
  m_renderSystem.onRender(renderer, isMinimized, currentTime);
  m_nativeScriptSystem.onRender(renderer, isMinimized, currentTime);
  m_imGuiSystem.onRender(renderer, isMinimized, currentTime);
  m_luaSystem.onRender(renderer, isMinimized, currentTime);
}

} // namespace pain
