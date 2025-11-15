
#pragma once
#include "ECS/Registry/Entity.h"
#include "Physics/Collision/GridCell.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <unordered_map>

namespace pain
{

struct GridManager {
  float m_cellSize = 1.0f;
  int m_defaultReserved = 8;
  float getCellSize() const { return m_cellSize; };
  std::unordered_map<int64_t, GridCell> m_grid = {};
  std::unordered_map<int64_t, GridCell> m_staticGrid = {};

  int64_t getKey(int32_t x, int32_t y) const;
  GridCell &getCell(int32_t x, int32_t y,
                    std::unordered_map<int64_t, GridCell> &grid);
  explicit GridManager(float cellSize = 1.0f);

  void clearAllDynamics();
  void clearDynamic(const glm::vec3 &pos);
  void insertStatic(const ColliderComponent &cc, TransformComponent &tc,
                    MovementComponent &mc);
  void insertDynamic(const ColliderComponent &cc, TransformComponent &tc,
                     MovementComponent &mc);
};

} // namespace pain
