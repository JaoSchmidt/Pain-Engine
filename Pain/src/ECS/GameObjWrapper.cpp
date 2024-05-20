#include "ECS/GameObjWrapper.h"

namespace pain
{

GameObjWrapper::GameObjWrapper(Scene &scene) : m_Scene(scene)
{
  m_entity = scene.createEntity();
}

GameObjWrapper::~GameObjWrapper() { m_Scene.destroyEntity(m_entity); }

} // namespace pain
