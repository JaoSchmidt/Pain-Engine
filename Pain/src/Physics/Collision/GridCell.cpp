#include "Physics/Collision/GridCell.h" // or GridCell
#include "ECS/Components/Movement.h"

namespace pain
{

GridCell::GridCell(int reserved)
{
  m_entities.reserve(reserved);
  m_components.reserve(reserved);
}

void GridCell::clear()
{
  m_entities.clear();
  m_components.clear();
}

void GridCell::push_back(Entity e, const ColliderComponent &cc,
                         const TransformComponent &tc)
{
  m_entities.push_back(e);
  m_components.push_back(std::make_tuple(&cc, &tc));
}

std::vector<std::tuple<const ColliderComponent *,
                       const TransformComponent *>>::const_iterator
GridCell::begin() const
{
  return m_components.begin();
}
std::vector<std::tuple<const ColliderComponent *,
                       const TransformComponent *>>::const_iterator
GridCell::end() const
{
  return m_components.end();
}

} // namespace pain
