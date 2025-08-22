#pragma once

#include "Core.h"

#include "ECS/Scene.h"

namespace pain
{

class SceneManager
{
public:
  NONCOPYABLE(SceneManager)
  SceneManager() = default;
  ~SceneManager();

  void addScene(const std::string &name, Scene *scene);
  // void popScene(Scene *scene);
  void popScene(const std::string &name);

  const Scene &getScene(const std::string &name) const;
  Scene &getScene(const std::string &);

  void attachScene(const std::string &name);
  void detachScene(const std::string &name);

  std::unordered_set<Scene *>::iterator begin();
  std::unordered_set<Scene *>::iterator end();

private:
  std::unordered_map<std::string, Scene *> m_scenes = {};
  std::unordered_set<Scene *> m_currentScenes = {};
};
} // namespace pain
