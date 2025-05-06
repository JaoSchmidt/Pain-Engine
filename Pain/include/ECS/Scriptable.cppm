module;
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
export module pain.Scriptable;
import pain.Entity;
import <tuple>;
import pain.Scene;

export namespace pain
{

class ScriptableEntity
{
public:
  Entity m_entity = 0;
  Scene *m_scene = nullptr;
  // const double deltaTimeSec = 1.0 / 60.0;
};

// Extended ScriptableEntity, meaning that it has additional functions to more
// easily get components
template <typename... Components>
class ExtendedScriptableEntity : public ScriptableEntity
{
public:
  // return the components of an entity, as a tuple
  std::tuple<Components &...> getAllComponents()
  {
    return m_scene->getAllComponents<Components...>(m_entity);
  }
  // return the components of an entity, as a tuple
  std::tuple<Components &...> getAllComponents() const
  {
    return m_scene->getAllComponents<Components...>(m_entity);
  }

  // return a single component of an entity
  template <typename T> T &getComponent()
  {
    return m_scene->getComponent<T, Components...>(m_entity);
  }
  // return a single component of an entity
  template <typename T> const T &getComponent() const
  {
    return m_scene->getComponent<T, Components...>(m_entity);
  }

  // TODO: In theory, it is possible to create the function "getSomeComponents",
  // maybe using passing the target components as argument by reference:
  // /*
  // template <typename... Components, typename... Args> void
  // getSomeComponents(Args &... args)
  // */
  // However, I'm not 100% sure so I'll do it later

  friend class Scene;
};
} // namespace pain
