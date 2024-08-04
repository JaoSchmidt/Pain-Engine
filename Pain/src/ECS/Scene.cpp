#include "ECS/Scene.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/NativeScript.h"
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
  m_registry->addComponentMap<NativeScriptComponent>();
}

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
  m_registry->remove<NativeScriptComponent>(entity);
  m_availableEntities.push(entity);
}
void Scene::spriteSystem()
{
  // and finally all sprites are rendered in this render method
  // to their appropriate position on the screen
  for (auto it = begin<SpriteComponent>(); it != end<SpriteComponent>(); ++it) {
    const TransformComponent &tc = getComponent<TransformComponent>(it->first);
    const SpriteComponent &sc = it->second;

    Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_ptexture,
                         sc.m_tilingFactor, sc.m_color);
  }
  for (auto it = begin<SpritelessComponent>(); it != end<SpritelessComponent>();
       ++it) {
    const TransformComponent &tc = getComponent<TransformComponent>(it->first);
    const SpritelessComponent &sc = it->second;
    Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_color);
  }
}

void Scene::rotationSystem()
{
  for (auto it = begin<RotationComponent>(); it != end<RotationComponent>();
       ++it) {
    RotationComponent &rc = it->second;
    rc.m_rotation = {cos(glm::radians(rc.m_rotationAngle)),
                     sin(glm::radians(rc.m_rotationAngle)), 0};
  }
}

void Scene::movementSystem(double dt)
{
  for (auto it = begin<MovementComponent>(); it != end<MovementComponent>();
       ++it) {
    const MovementComponent &mc = it->second;
    TransformComponent &tc = getComponent<TransformComponent>(it->first);
    const float moveAmount = (float)(dt * mc.m_translationSpeed);
    tc.m_position = tc.m_position + mc.m_velocityDir * moveAmount;
  }
}

void Scene::scriptSystem(double dt)
{
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
      nsc.onUpdateFunction(nsc.instance, dt);
  }
}

void Scene::scriptSystem(const SDL_Event &event)
{
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    auto &nsc = it->second;
    nsc.onEventFunction(nsc.instance, event);
  }
}

} // namespace pain
