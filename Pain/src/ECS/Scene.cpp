#include "ECS/Scene.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Components/Particle.h"
#include "ECS/Registry.h"

#include "ECS/Components/Movement.h"
#include "ECS/Components/Rotation.h"
#include "ECS/Components/Sprite.h"
#include "SDL_events.h"

namespace pain
{

Scene::Scene() : m_registry(new Registry())
{
  m_registry->addComponentMap<OrthoCameraComponent>();
  m_registry->addComponentMap<TransformComponent>();
  m_registry->addComponentMap<MovementComponent>();
  m_registry->addComponentMap<RotationComponent>();
  m_registry->addComponentMap<SpriteComponent>();
  m_registry->addComponentMap<SpritelessComponent>();
  m_registry->addComponentMap<TrianguleComponent>();
  m_registry->addComponentMap<NativeScriptComponent>();
  m_registry->addComponentMap<ParticleSprayComponent>();
}
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

void Scene::destroyEntity(Entity entity)
{
  // m_registry->removeAll(entity);
  m_registry->remove<OrthoCameraComponent>(entity);
  m_registry->remove<MovementComponent>(entity);
  m_registry->remove<RotationComponent>(entity);
  m_registry->remove<TransformComponent>(entity);
  m_registry->remove<SpriteComponent>(entity);
  m_registry->remove<SpritelessComponent>(entity);
  m_registry->remove<TrianguleComponent>(entity);
  m_registry->remove<NativeScriptComponent>(entity);
  m_registry->remove<ParticleSprayComponent>(entity);
  m_availableEntities.push(entity);
}

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

void Scene::renderSystems(double currentTime)
{
  // and finally all sprites are rendered in this render method
  // to their appropriate position on the screen
  for (auto it = begin<SpriteComponent>(); it != end<SpriteComponent>(); ++it) {
    const TransformComponent &tc = getComponent<TransformComponent>(it->first);
    const SpriteComponent &sc = it->second;
    if (hasComponent<RotationComponent>(it->first)) {
      const RotationComponent &rc = getComponent<RotationComponent>(it->first);
      Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_color,
                           rc.m_rotationAngle, sc.m_ptexture,
                           sc.m_tilingFactor);
      // TODO: Remove m_rotation of rc... should only
      // have angle, in the case of the camera
      // inclune rot direction in its script
    } else {
      Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_color, sc.m_ptexture,
                           sc.m_tilingFactor);
    }
  }

  for (auto it = begin<SpritelessComponent>(); it != end<SpritelessComponent>();
       ++it) {
    const TransformComponent &tc = getComponent<TransformComponent>(it->first);
    const SpritelessComponent &sc = it->second;
    Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_color);
  }

  for (auto it = begin<TrianguleComponent>(); it != end<TrianguleComponent>();
       ++it) {
    const TransformComponent &tc = getComponent<TransformComponent>(it->first);
    const TrianguleComponent &sc = it->second;
    Renderer2d::drawTri(tc.m_position, sc.m_height, sc.m_color);
  }

  // =============================================================== //
  // Render Native Script Components
  // =============================================================== //
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    auto &nsc = it->second;
    // NOTE: If there is a future where Events runs before Updates, uncomment
    // this instance check
    P_ASSERT(
        nsc.instance != nullptr,
        "The nsc.instance should be instanciated before calling on render");
    if (nsc.onRenderFunction)
      nsc.onRenderFunction(nsc.instance, currentTime);
  }

  // =============================================================== //
  // Render Particle Systems
  // =============================================================== //
  for (auto it = begin<ParticleSprayComponent>();
       it != end<ParticleSprayComponent>(); ++it) {
    ParticleSprayComponent &psc = it->second;
    Renderer2d::beginSprayParticle(currentTime, psc.m_velocity,
                                   psc.m_emiterPosition);
    for (Particle &pa : psc.m_particles) {
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
    RotationComponent &rc = it->second;
    rc.m_rotation = {cos(rc.m_rotationAngle), sin(rc.m_rotationAngle), 0};
  }

  // =============================================================== //
  // Update Movement Components
  // =============================================================== //
  for (auto it = begin<MovementComponent>(); it != end<MovementComponent>();
       ++it) {
    const MovementComponent &mc = it->second;
    TransformComponent &tc = getComponent<TransformComponent>(it->first);
    const float moveAmount = (float)(deltaTime * mc.m_translationSpeed);
    tc.m_position = tc.m_position + mc.m_velocityDir * moveAmount;
  }

  // =============================================================== //
  // Update Native Script Components
  // =============================================================== //
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    auto &nsc = it->second;
    if (!nsc.instance) {
      nsc.instantiateFunction(nsc.instance);
      nsc.instance->m_scene = this;
      nsc.instance->m_entity = it->first;

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
    auto &nsc = it->second;
    // NOTE: If there is a future where Events runs before Updates, uncomment
    // this instance check
    P_ASSERT(nsc.instance != nullptr,
             "The nsc.instance should be instanciated before updating events");
    // if (!nsc.instance) {
    //   nsc.instantiateFunction(nsc.instance);
    //   nsc.instance->m_scene = this;
    //   nsc.instance->m_entity = it->first;

    //   if (nsc.onCreateFunction)
    //     nsc.onCreateFunction(nsc.instance);
    // }
    if (nsc.onEventFunction)
      nsc.onEventFunction(nsc.instance, event);
  }
}

} // namespace pain
