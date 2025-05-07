module;
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include <SDL_events.h>
#include <tuple>
export module pain.Scene;
import pain.Entity;
import pain.ArcheRegistry;
import pain.IteratorArchetype;

export namespace pain
{

class Scene
{
private:
  ArcheRegistry *m_registry;

public:
  Scene();
  ~Scene() { delete m_registry; };

  Entity createEntity();
  void destroyEntity(Entity entity);

  virtual void onRender(double realTime) = 0;
  virtual void onUpdate(double deltaTime) = 0;
  virtual void onEvent(const SDL_Event &event) = 0;

  // ---------------------------------------------------- //
  // Component Archetype stuff
  // ---------------------------------------------------- //
  template <typename... Components, typename... Args>
  std::tuple<Components &...> createComponents(Entity entity, Args &&...args)
  {
    return m_registry->createComponents<Components...>(
        entity, std::forward<Args>(args)...);
  }
  template <typename T, typename... Args>
  T &createComponent(Entity entity, Args &&...args)
  {
    P_ASSERT_W(!hasComponent<T>(entity), "Entity {} already has component !",
               entity);
    return m_registry->createComponent<T>(entity, std::forward<Args>(args)...);
  }

  // ---------------------------------------------------- //
  // Get components from archetypes
  // ---------------------------------------------------- //
  // get multiple components together as a tuple
  template <typename... Components>
  std::tuple<Components &...> getAllComponents(Entity entity)
  {
    return m_registry->getAllComponents<Components...>(entity);
  }
  // get multiple components together as a tuple
  template <typename... Components>
  const std::tuple<const Components &...> getAllComponents(Entity entity) const
  {
    return static_cast<const ArcheRegistry *>(m_registry)
        ->getAllComponents<Components...>(entity);
  }

  // get a single component
  template <typename T, typename... Components> T &getComponent(Entity entity)
  {
    return m_registry->getComponent<T, Components...>(entity);
  }
  // get a single component
  template <typename T, typename... Components>
  const T &getComponent(Entity entity) const
  {
    return static_cast<const ArcheRegistry *>(m_registry)
        ->getComponent<T, Components...>(entity);
  }

  template <typename... TargetComponents, typename... ObjectComponents>
  bool hasComponent(Entity entity) const
  {
    return m_registry->has<TargetComponents..., ObjectComponents...>(entity);
  }
  // remove an entity, alongside its components from it's archetype
  template <typename... ObjectComponents> bool removeEntity(Entity entity)
  {
    return m_registry->remove<ObjectComponents...>(entity);
  }
  // Remove components of an entity
  // template <typename... TargetComponents, typename... ObjectComponents>
  // void rmComponent(Entity entity)
  // {
  //   m_registry->remove<TargetComponents..., ObjectComponents...>(entity);
  // }

  // ---------------------------------------------------- //
  // Iterate archetypes
  // ---------------------------------------------------- //

  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> end()
  {
    return m_registry->end<T>();
  }
  template <typename T>
    requires IsSingleType<T>
  reg::Iterator<T> begin()
  {
    return m_registry->begin<T>();
  }
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...> begin()
  {
    return m_registry->begin<Components...>();
  }
  template <typename... Components>
    requires IsMultipleTypes<Components...>
  std::tuple<reg::Iterator<Components>...> end()
  {
    return m_registry->end<Components...>();
  }

  // ---------------------------------------------------- //
  // Systems
  // ---------------------------------------------------- //

  void updateSystems(double deltaTime);
  void updateSystems(const SDL_Event &event);
  void renderSystems(double currentTime);

private:
  inline static std::queue<Entity> m_availableEntities = {};
  inline static Entity numberOfEntities = 0;
};
} // namespace pain
