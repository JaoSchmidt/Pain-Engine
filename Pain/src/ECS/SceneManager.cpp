#include "ECS/SceneManager.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Scene.h"
#include <unordered_map>

namespace pain
{

void SceneManager::addScene(const std::string &name, Scene *scene)
{
  m_scenes[name] = scene;
}

void SceneManager::popScene(const std::string &name)
{
  P_ASSERT_W(m_scenes.find(name) != m_scenes.end(),
             "Trying to remove a scene that doesn't exist")
  m_scenes.erase(name);
}

Scene &SceneManager::getScene(const std::string &name)
{
  P_ASSERT_W(m_scenes.find(name) != m_scenes.end(),
             "Trying to access scene that doesn't exist");
  return *m_scenes.at(name);
}

const Scene &SceneManager::getScene(const std::string &name) const
{
  P_ASSERT_W(m_scenes.find(name) != m_scenes.end(),
             "Trying to access scene that doesn't exist");
  return *m_scenes.at(name);
}

void SceneManager::attachScene(const std::string &name)
{
  if (m_scenes.find(name) != m_scenes.end() &&
      m_currentScenes.find(m_scenes.at(name)) == m_currentScenes.end()) {
    m_currentScenes.insert(m_scenes.at(name));
  }
}

void SceneManager::detachScene(const std::string &name)
{
  if (m_scenes.find(name) != m_scenes.end() &&
      m_currentScenes.find(m_scenes.at(name)) != m_currentScenes.end()) {
    m_currentScenes.erase(m_scenes.at(name));
  }
}

std::unordered_set<Scene *>::iterator SceneManager::begin()
{
  return m_currentScenes.begin();
}
std::unordered_set<Scene *>::iterator SceneManager::end()
{
  return m_currentScenes.end();
}
} // namespace pain
