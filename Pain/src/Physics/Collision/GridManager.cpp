#include "Physics/Collision/GridManager.h"

namespace pain
{
/* Grid Manager approach: As with every approach, this has steps:
 *
 * we must first run a single loop of insertion inside the system, it will be
 * responsable for copying all sizes, offsets and entity ids to the grid cells
 * if an entity intersect x abstract cells, just insert the same entity into
 * those x
 *
 * now, every chunk/cell is capable of checking the collisions within itself
 */

GridManager::GridManager(float cellSize) : m_cellSize(cellSize) {}

int64_t GridManager::getKey(int32_t x, int32_t y) const
{
  return (static_cast<int64_t>(x) << 32) | static_cast<int64_t>(y);
}
GridCell &GridManager::getCell(int32_t x, int32_t y,
                               std::unordered_map<int64_t, GridCell> &grid)
{
  auto [it, inserted] = m_grid.try_emplace(getKey(x, y), m_defaultReserved);
  return it->second;
}

GridCellStatic &
GridManager::getCellStatic(int32_t x, int32_t y,
                           std::unordered_map<int64_t, GridCellStatic> &grid)
{
  auto [it, inserted] =
      m_staticGrid.try_emplace(getKey(x, y), m_defaultReserved);
  return it->second;
}

// NOTE: idk if this is the function I should use on the collision system, or
// use the clearDynamic(<chunk pos>) individually
void GridManager::clearAllDynamics()
{
  for (auto &[key, entitiesVec] : m_grid) {
    // NOTE: clear destroy elements, but keep space allocated
    entitiesVec.clear();
  }
}

// NOTE: idk if this is the function I should use on the collision system, or
// clear everything with clearAllDynamics
void GridManager::clearDynamic(const glm::vec3 &pos)
{
  // TODO: Calculate using size to check the case a single entity is inside
  // multiple cells
  int x = static_cast<int>(pos.x / m_cellSize);
  int y = static_cast<int>(pos.y / m_cellSize);
  getCell(x, y, m_grid).clear();
}
void GridManager::insertStatic(const ColliderComponent &cc,
                               TransformComponent &tc)
{
  // TODO: Calculate using size to check the case a single entity is inside
  // multiple cells
  int x = static_cast<int>(tc.m_position.x / m_cellSize);
  int y = static_cast<int>(tc.m_position.y / m_cellSize);
  getCellStatic(x, y, m_staticGrid).push_back(cc, tc);
}

void GridManager::insertDynamic(const ColliderComponent &cc,
                                TransformComponent &tc, MovementComponent &mc)
{
  int x = static_cast<int>(tc.m_position.x / m_cellSize);
  int y = static_cast<int>(tc.m_position.y / m_cellSize);
  getCell(x, y, m_grid).push_back(cc, tc, mc);
}

} // namespace pain
