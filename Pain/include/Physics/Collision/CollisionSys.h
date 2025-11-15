#pragma once

#include "ECS/Components/Movement.h"
#include "ECS/Systems.h"
#include "Physics/Collision/GridManager.h"
namespace pain
{

namespace Systems
{
struct CollisionSystem : public System<ComponentManager> {
  template <typename... Args>
  CollisionSystem(float cellSize, Args &&...args)
      : System(std::forward<Args>(args)...), m_gridManager(cellSize){};
  CollisionSystem() = delete;
  void onUpdate(double dt);

  float getCellSize() const { return m_gridManager.getCellSize(); }
  GridManager m_gridManager;
  void solidCollision(glm::vec3 &pos1, //
                      glm::vec3 &vel1, //
                      glm::vec3 &pos2, //
                      glm::vec3 &vel2, //
                      float mass1 = 1.f, float mass2 = 1.f);
};

} // namespace Systems
} // namespace pain
