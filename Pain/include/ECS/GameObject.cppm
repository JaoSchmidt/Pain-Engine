module;
#include "Core.h"
#include "Debugging/Profiling.h"
#include <tuple>
export module pain.GameObject;
import pain.ArcheRegistry;
import pain.Scene;
import pain.Entity;

export namespace pain
{

// Allows easy manipulation of a game object without writing long functions
template <typename... ObjectComponents> class GameObject
{
public:
  inline Entity getEntity() const { return m_entity; }
  GameObject(Scene *scene) : m_scene(scene)
  {
    m_entity = scene->createEntity();
  }

  // Remove the entity, alongside its components from it's archetype
  ~GameObject()
  {
    m_scene->removeEntity<ObjectComponents...>(m_entity);
    m_scene->destroyEntity(m_entity);
  }
  template <typename... Components>
  std::tuple<Components &...> createComponents(Components &&...args)
  {
    return m_scene->createComponents<Components...>(
        m_entity, std::forward<Components>(args)...);
  }
  template <typename T, typename... Args> T &createComponent(Args &&...args)
  {
    return m_scene->createComponent<T>(m_entity, std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  std::tuple<ObjectComponents &...> getAllComponents()
  {
    PROFILE_FUNCTION();
    return m_scene->getAllComponents<ObjectComponents...>(m_entity);
  }
  const std::tuple<ObjectComponents &...> getAllComponents() const
  {
    PROFILE_FUNCTION();
    return static_cast<const Scene *>(m_scene)
        ->getAllComponents<ObjectComponents...>(m_entity);
  }
  template <typename T> T &getComponent()
  {
    PROFILE_FUNCTION();
    return m_scene->getComponent<T, ObjectComponents...>(m_entity);
  }
  template <typename T> const T &getComponent() const
  {
    PROFILE_FUNCTION();
    return static_cast<const Scene *>(m_scene)
        ->getComponent<T, ObjectComponents...>(m_entity);
  }
  // ---------------------------------------------------- //
  // Does archetype has components?
  // ---------------------------------------------------- //
  template <typename... TargetComponents> bool hasComponents() const
  {
    PROFILE_FUNCTION();
    return m_scene->hasComponent<TargetComponents..., ObjectComponents...>(
        m_entity);
  }

  // Remove components of an entity
  // template <typename... TargetComponets, typename... Components>
  // void rmComponents()
  // {
  //   m_scene->rmComponent<TargetComponets..., ObjectComponents...>(m_entity);
  // }

  explicit operator bool() const { return m_entity != -1; }

protected:
  template <typename arg> void initializeScript(arg &&nativeScriptComponent)
  {
    initializeScripts(m_scene, std::forward<arg>(nativeScriptComponent),
                      m_entity);
  }
  Entity m_entity = -1;
  Scene *m_scene;
  friend class Scene;
};

} // namespace pain
