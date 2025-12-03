
#include "ECS/Scene.h"

#include "CoreRender/RenderSys.h"
#include "Debugging/Profiling.h"
#include "GUI/ImGuiSys.h"
#include "Physics/Collision/CollisionNaiveSys.h"
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
      m_collisionSystem{new Systems::NaiveCollisionSys(m_registry)} {};

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
  m_collisionSystem->onUpdate(deltaTime);
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
