#include "ECS/GameObject.h"

namespace pain
{

GameObject::GameObject(Scene *scene) : m_scene(scene)
{
  m_entity = scene->createEntity();
}

GameObject::~GameObject() { m_scene->destroyEntity(m_entity); }

} // namespace pain
