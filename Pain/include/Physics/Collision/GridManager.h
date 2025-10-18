
#pragma once
#include "ECS/Entity.h"
#include "Physics/Collision/GridCell.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>
#include <vector>

namespace pain
{

struct GridManager {
  float m_cellSize;
  std::unordered_map<int64_t, GridCell> m_grid;
  std::unordered_map<int64_t, GridCell> m_staticGrid;

  int64_t getKey(int x, int y) const;
  explicit GridManager(float cellSize = 1.0f);

  void clearAllDynamics();
  void clearDynamic(const glm::vec3 &pos);
  void insertStatic(Entity entity, const ColliderComponent &cc,
                    const TransformComponent &tc);
  void insertDynamic(Entity entity, const ColliderComponent &cc,
                     const TransformComponent &tc);
};

} // namespace pain
