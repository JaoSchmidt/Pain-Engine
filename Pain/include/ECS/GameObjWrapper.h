#pragma once
#include "pch.h"

#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/Scene.h"

namespace pain
{

class EXPORT GameObjWrapper
{
public:
  GameObjWrapper(Scene &scene);
  virtual ~GameObjWrapper();

  template <typename T, typename... Args> T &addComponent(Args &&...args)
  {
    PLOG_W(!hasComponent<T>(), "Entity already has component!");
    return m_Scene.addComponent<T, Args...>(m_entity,
                                            std::forward<Args>(args)...);
  }

  template <typename T> T &getComponent()
  {
    return m_Scene.getComponent<T>(m_entity);
  }

  template <typename T> bool hasComponent()
  {
    return m_Scene.hasComponent<T>(m_entity);
  }

  template <typename T> void rmComponent() { m_Scene.rmComponent<T>(m_entity); }

  operator bool() const { return m_entity != -1; }

private:
  Entity m_entity = -1;
  Scene &m_Scene;
};
} // namespace pain
