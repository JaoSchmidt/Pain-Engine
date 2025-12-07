
#include "ECS/Scene.h"

#include "CoreRender/RenderSys.h"
#include "Debugging/Profiling.h"
#include "ECS/Scriptable.h"
#include "GUI/ImGuiSys.h"
#include "Physics/Collision/Collider.h"
#include "Physics/Collision/CollisionNaiveSys.h"
#include "Physics/Collision/SweepAndPruneSys.h"
#include "Physics/KinematicsSys.h"
#include "Scripting/LuaScriptSys.h"
#include "Scripting/NativeSystem.h"

namespace pain
{

Scene::Scene(sol::state &solState, void *context, SDL_Window *window)

    : m_registry(), m_luaState(solState), m_entity(createEntity()),
      // ---- Systems --------------------------------
      m_renderSystem{new Systems::Render(m_registry)},
      m_kinematicsSystem{new Systems::Kinematics(m_registry)},
      m_nativeScriptSystem{new Systems::NativeScript(m_registry)},
      m_imGuiSystem{new Systems::ImGuiSys(m_registry, context, window)},
      m_luaSystem{new Systems::LuaScript(m_registry)},
      m_sweepAndPruneSystem{new Systems::SweepAndPruneSys(m_registry)}
{
  SAPCollider::s_systemsReference = m_sweepAndPruneSystem;
};

void Scene::insertColliders(const std::vector<reg::Entity> &entities)
{
  // TODO: we can probably extract the components faster
  // - perhaps by requiring entities to be in order and make a "merge walk" with
  // std::vector<Records> in the registry?
  // HACK: Why is this slow? because you are looking for the archetype for every
  // iteration.
  for (const reg::Entity &entity : entities) {
    auto [tc, sc] = getComponents<Transform2dComponent, SAPCollider>(entity);
    if (hasAnyComponents<Movement2dComponent>(entity))
      m_sweepAndPruneSystem->insertEntity(entity, tc, sc);
    else
      m_sweepAndPruneSystem->insertStaticEntity(entity, tc, sc);
  }
  m_sweepAndPruneSystem->sortAfterInsertion();
}

// clang-format off
      // m_renderSystem{std::make_unique<Systems::Render>(m_registry)},
      // m_kinematicsSystem{std::make_unique<Systems::Kinematics>(m_registry)},
      // m_nativeScriptSystem{std::make_unique<Systems::NativeScript>(m_registry)},
      // m_imGuiSystem{std::make_unique<Systems::ImGuiSys>(m_registry, context, window)},
      // m_luaSystem{std::make_unique<Systems::LuaScript>(m_registry)},
      // m_collisionSystem{std::make_unique<Systems::NaiveCollisionSys>(m_registry)} {};

void Scene::updateSystems(double deltaTime)
{
  PROFILE_FUNCTION();
  m_kinematicsSystem->onUpdate(deltaTime);
  m_nativeScriptSystem->onUpdate(deltaTime);
  m_luaSystem->onUpdate(deltaTime);
  m_sweepAndPruneSystem->onUpdate(deltaTime);
}

void Scene::updateSystems(const SDL_Event &event)
{
  PROFILE_SCOPE("Scene::updateSystems - events for nsc");
  m_nativeScriptSystem->onEvent(event);
  m_imGuiSystem->onEvent(event);
  m_luaSystem->onEvent(event);
}
//
// void Scene::insertStaticCollider(reg::Entity entity)
// {
//   // m_collisionSystem.insertStatic(
//   //     entity, getComponent<TransformComponent>(entity).m_position);
// }

void Scene::renderSystems(Renderer2d &renderer, bool isMinimized,
                          double currentTime)
{
  m_renderSystem->onRender(renderer, isMinimized, currentTime);
  m_nativeScriptSystem->onRender(renderer, isMinimized, currentTime);
  m_imGuiSystem->onRender(renderer, isMinimized, currentTime);
  m_luaSystem->onRender(renderer, isMinimized, currentTime);
}

} // namespace pain
