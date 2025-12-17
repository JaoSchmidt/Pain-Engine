
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
#include "Scripting/NativeScriptSys.h"

namespace pain
{

Scene::Scene(sol::state &solState, void *context, SDL_Window *window)

    : m_registry(), m_eventDispatcher(), m_luaState(solState),
      m_entity(createEntity()),
      // ---- Systems --------------------------------
      // clang-format off
      m_renderSystem{new Systems::Render(m_registry, m_eventDispatcher)},
      m_kinematicsSystem{new Systems::Kinematics(m_registry, m_eventDispatcher)},
      m_nativeScriptSystem{new Systems::NativeScript(m_registry, m_eventDispatcher)},
      m_imGuiSystem{new Systems::ImGuiSys(m_registry,m_eventDispatcher, context, window)},
      m_luaSystem{new Systems::LuaScript(m_registry, m_eventDispatcher)},
      m_sweepAndPruneSystem{new Systems::SweepAndPruneSys(m_registry, m_eventDispatcher)} // clang-format on
{};

// clang-format off
      // m_renderSystem{std::make_unique<Systems::Render>(m_registry)},
      // m_kinematicsSystem{std::make_unique<Systems::Kinematics>(m_registry)},
      // m_nativeScriptSystem{std::make_unique<Systems::NativeScript>(m_registry)},
      // m_imGuiSystem{std::make_unique<Systems::ImGuiSys>(m_registry, context, window)},
      // m_luaSystem{std::make_unique<Systems::LuaScript>(m_registry)},
      // m_collisionSystem{std::make_unique<Systems::NaiveCollisionSys>(m_registry)} {};

void Scene::updateSystems(DeltaTime deltaTime)
{
  PROFILE_FUNCTION();
  m_kinematicsSystem->onUpdate(deltaTime);
  m_nativeScriptSystem->onUpdate(deltaTime);
  m_luaSystem->onUpdate(deltaTime);
  m_sweepAndPruneSystem->onUpdate(deltaTime);
  m_eventDispatcher.update();
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
                          DeltaTime currentTime)
{
  m_renderSystem->onRender(renderer, isMinimized, currentTime);
  m_nativeScriptSystem->onRender(renderer, isMinimized, currentTime);
  m_imGuiSystem->onRender(renderer, isMinimized, currentTime);
  m_luaSystem->onRender(renderer, isMinimized, currentTime);
}

Scene::~Scene(){
  delete m_renderSystem;
  delete m_kinematicsSystem;
  delete m_nativeScriptSystem;
  delete m_imGuiSystem;
  delete m_luaSystem;
  delete m_sweepAndPruneSystem;
}

} // namespace pain
