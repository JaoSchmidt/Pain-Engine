#pragma once
#include "ECS/Components/Movement.h"
#include "ECS/Registry/Entity.h"
#include "Physics/Collision/Collider.h"
#include "glm/ext/vector_float2.hpp"
#include <vector>

namespace pain
{
// NOTE: this is highly adapted to rectangles, maybe I will need a different
// strategy for different shapes. Possibly a tree like in the Reducible video:
// https://www.youtube.com/watch?v=eED4bSkYCB8
struct GridCell {
  std::vector<std::tuple<const ColliderComponent *, TransformComponent *,
                         MovementComponent *>>
      m_components;
  GridCell(int reserved);
  void clear();
  void push_back(const ColliderComponent &cc, TransformComponent &tc,
                 MovementComponent &mc);

  std::vector<std::tuple<const ColliderComponent *, TransformComponent *,
                         MovementComponent *>>::const_iterator
  begin() const;
  std::vector<std::tuple<const ColliderComponent *, TransformComponent *,
                         MovementComponent *>>::const_iterator
  end() const;
};
struct GridCellStatic {
  std::vector<std::tuple<const ColliderComponent *, TransformComponent *>>
      m_components;
  GridCellStatic(int reserved);
  void clear();
  void push_back(const ColliderComponent &cc, TransformComponent &tc);

  std::vector<std::tuple<const ColliderComponent *,
                         TransformComponent *>>::const_iterator
  begin() const;
  std::vector<std::tuple<const ColliderComponent *,
                         TransformComponent *>>::const_iterator
  end() const;
};

using Chunk = GridCell;

} // namespace pain
