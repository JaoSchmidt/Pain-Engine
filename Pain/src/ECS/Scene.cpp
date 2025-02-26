#include "ECS/Scene.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Components/Particle.h"

#include "ECS/Components/Movement.h"
#include "ECS/Components/Rotation.h"
#include "ECS/Components/Sprite.h"
#include "SDL_events.h"

namespace pain
{

Scene::Scene() : m_registry(new ArcheRegistry()) {}
// TODO: Create way to move and copy components to another scene

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

// TODO: I may need to fix this
void Scene::destroyEntity(Entity entity) { m_availableEntities.push(entity); }

// =============================================================== //
// Systems
// =============================================================== //
void Scene::initializeScripts(NativeScriptComponent &nsc, const GameObject &go)
{
  if (!nsc.instance) {
    nsc.instantiateFunction(nsc.instance);
    nsc.instance->m_scene = this;
    nsc.instance->m_entity = go.m_entity;

    if (nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }
}
void Scene::initializeScripts(NativeScriptComponent &nsc, Entity e)
{
  if (!nsc.instance) {
    nsc.instantiateFunction(nsc.instance);
    nsc.instance->m_scene = this;
    nsc.instance->m_entity = e;

    if (nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
  }
}

// =============================================================== //
// Render Components
// =============================================================== //

// This function is responsible for iterating and rendering all components that
// need to be rendered, such as Sprites and Spriteless components
void Scene::renderSystems(double currentTime)
{
  {
    auto [tIt, sIt] = begin<TransformComponent, SpriteComponent>();
    const auto &[tItEnd, sItEnd] = end<TransformComponent, SpriteComponent>();

    for (; tIt != tItEnd; ++tIt, ++sIt) {
      const TransformComponent &tc = *tIt;
      const SpriteComponent &sc = *sIt;
      Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_color, sc.m_ptexture,
                           sc.m_tilingFactor);
    }
  }
  {
    auto [tIt, sIt, rIt] =
        begin<TransformComponent, SpriteComponent, RotationComponent>();
    const auto &[tItEnd, sItEnd2, rItEnd] =
        end<TransformComponent, SpriteComponent, RotationComponent>();

    for (; tIt != tItEnd; ++tIt, ++rIt, ++sIt) {
      Renderer2d::drawQuad(tIt->m_position, sIt->m_size, sIt->m_color,
                           rIt->m_rotationAngle, sIt->m_ptexture,
                           sIt->m_tilingFactor);
      // TODO: Remove m_rotation of rc... should only
      // have angle, in the case of the camera
      // inclune rot direction in its script
    }
  }
  {
    auto [tIt, sIt] = begin<TransformComponent, SpriteComponent>();
    auto [tItEnd, sItEnd] = end<TransformComponent, SpriteComponent>();
    for (; tIt != tItEnd; ++tIt, ++sIt) {
      Renderer2d::drawQuad(tIt->m_position, sIt->m_size, sIt->m_color);
    }
  }
  {
    auto [tIt, triIt] = begin<TransformComponent, TrianguleComponent>();
    auto [tItEnd, triItEnd] = end<TransformComponent, TrianguleComponent>();
    for (; tIt != tItEnd; ++tIt, ++triIt) {
      Renderer2d::drawTri(tIt->m_position, triIt->m_height, triIt->m_color);
    }
  }

  // =============================================================== //
  // Render Native Script Components
  // =============================================================== //
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    NativeScriptComponent &nsc = *it;
    if (!nsc.instance) {
      nsc.instantiateFunction(nsc.instance);
      nsc.instance->m_scene = this;
      nsc.instance->m_entity = it.getEntity();

      if (nsc.onCreateFunction)
        nsc.onCreateFunction(nsc.instance);
    }

    if (nsc.onRenderFunction)
      nsc.onRenderFunction(nsc.instance, currentTime);
  }

  // =============================================================== //
  // Render Particle Systems
  // =============================================================== //
  for (auto it = begin<ParticleSprayComponent>();
       it != end<ParticleSprayComponent>(); ++it) {
    ParticleSprayComponent &psc = *it;
    Renderer2d::beginSprayParticle(currentTime, psc);
    for (Particle &pa : psc.m_particles) {
      if (pa.m_alive)
        Renderer2d::drawSprayParticle(pa);
      // Remove dead particles
      if (currentTime - pa.m_startTime >= psc.m_lifeTime) {
        pa.m_alive = false;
      }
    }
  }
}

void Scene::updateSystems(double deltaTime)
{
  // =============================================================== //
  // Update Rotation Components
  // =============================================================== //
  for (auto it = begin<RotationComponent>(); it != end<RotationComponent>();
       ++it) {
    RotationComponent &rc = *it;
    rc.m_rotation = {cos(rc.m_rotationAngle), sin(rc.m_rotationAngle), 0};
  }

  // =============================================================== //
  // Update Movement Components
  // =============================================================== //
  {
    auto [tIt, mIt] = begin<TransformComponent, MovementComponent>();
    auto [tItEnd, mItEnd] = end<TransformComponent, MovementComponent>();
    for (; tIt != tItEnd; ++tIt, ++mIt) {
      const float moveAmount = (float)(mIt->m_translationSpeed * deltaTime);
      tIt->m_position += mIt->m_velocityDir * moveAmount;
    }
  }

  // =============================================================== //
  // Update Native Script Components
  // =============================================================== //
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    auto &nsc = *it;
    if (!nsc.instance) {
      nsc.instantiateFunction(nsc.instance);
      nsc.instance->m_scene = this;
      nsc.instance->m_entity = it.getEntity();

      if (nsc.onCreateFunction)
        nsc.onCreateFunction(nsc.instance);
    }

    if (nsc.onUpdateFunction)
      nsc.onUpdateFunction(nsc.instance, deltaTime);
  }
}

void Scene::updateSystems(const SDL_Event &event)
{
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    auto &nsc = *it;
    if (!nsc.instance) {
      nsc.instantiateFunction(nsc.instance);
      nsc.instance->m_scene = this;
      nsc.instance->m_entity = it.getEntity();

      if (nsc.onCreateFunction)
        nsc.onCreateFunction(nsc.instance);
    }
    if (nsc.onEventFunction)
      nsc.onEventFunction(nsc.instance, event);
  }
}

} // namespace pain
