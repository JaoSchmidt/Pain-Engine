#include "ECS/GameObjWrapper.h"

namespace pain
{

GameObjWrapper::GameObjWrapper(Scene *scene) : m_scene(scene)
{
  m_entity = scene->createEntity();
}

GameObjWrapper::~GameObjWrapper() { m_scene->destroyEntity(m_entity); }

} // namespace pain
