#pragma once
#include "ECS/Components/Movement.h"
#include "ECS/Entity.h"
#include "Physics/Collision/Collider.h"
#include "glm/ext/vector_float2.hpp"
#include <vector>

namespace pain
{
// NOTE: this is highly adapted to rectangles, maybe I will need a different
// strategy for different shapes. Possibly a tree like in the Reducible video:
// https://www.youtube.com/watch?v=eED4bSkYCB8
struct GridCell {
  std::vector<Entity> m_entities;
  std::vector<std::tuple<const ColliderComponent *, const TransformComponent *>>
      m_components;
  GridCell(int reserved);
  void clear();
  void push_back(Entity e, const ColliderComponent &cc,
                 const TransformComponent &tc);

  std::vector<std::tuple<const ColliderComponent *,
                         const TransformComponent *>>::const_iterator
  begin() const;
  std::vector<std::tuple<const ColliderComponent *,
                         const TransformComponent *>>::const_iterator
  end() const;
};

using Chunk = GridCell;

} // namespace pain
