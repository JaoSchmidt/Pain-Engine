// Scriptable.cpp
#include "ECS/Scriptable.h"

namespace pain
{

// ------------------------
// Misc
// ------------------------
template <typename SceneT>
reg::EventDispatcher &GameObject<SceneT>::getEventDispatcher()
{
  return m_scene.get().getEventDispatcher();
}

template <typename SceneT>
const reg::EventDispatcher &GameObject<SceneT>::getEventDispatcher() const
{
  return std::as_const(m_scene.get()).getEventDispatcher();
}

template <typename SceneT> SceneT &GameObject<SceneT>::getScene()
{
  return m_scene.get();
}

template <typename SceneT> const SceneT &GameObject<SceneT>::getScene() const
{
  return std::as_const(m_scene).get();
}

template <typename SceneT> ThreadPool &GameObject<SceneT>::getThreadPool()
{
  return m_scene.get().getThreadPool();
}

template <typename SceneT>
const ThreadPool &GameObject<SceneT>::getThreadPool() const
{
  return std::as_const(m_scene.get()).getThreadPool();
}
// ------------------------------------------------------------
// Removal
// ------------------------------------------------------------

template <typename SceneT>
void GameObject<SceneT>::removeEntity(reg::Entity entity)
{
  m_scene.get().removeEntity(entity);
}

// ------------------------
// Constructor
// ------------------------
template <typename SceneT>
GameObject<SceneT>::GameObject(reg::Entity entity, SceneT &scene)
    : m_scene(scene), m_entity(entity){};

template <typename SceneT>
GameObject<SceneT> GameObject<SceneT>::create(reg::Entity entity, SceneT &scene)
{
  return GameObject<SceneT>{entity, scene};
}
// ------------------------------------------------------------
// Explicit instantiations
// ------------------------------------------------------------

template class GameObject<Scene>;
template class GameObject<UIScene>;

} // namespace pain
