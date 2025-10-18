// SpatialPartition.h
#pragma once
#include "ECS/Entity.h"
#include "Physics/Collision/GridManager.h"
#include "pch.h"

namespace pain
{

struct SpatialGridComponent {
  int m_gridX = 0;
  int m_gridY = 0;
  GridManager m_spatialGrid;

  SpatialGridComponent() = default;
  SpatialGridComponent(int x, int y) : m_gridX(x), m_gridY(y) {}
};

} // namespace pain
