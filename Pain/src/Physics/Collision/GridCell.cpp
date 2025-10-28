#include "Physics/Collision/GridCell.h" // or GridCell
#include "ECS/Components/Movement.h"

namespace pain
{

GridCell::GridCell(int reserved) { m_components.reserve(reserved); }

void GridCell::clear() { m_components.clear(); }

void GridCell::push_back(const ColliderComponent &cc, TransformComponent &tc,
                         MovementComponent &mc)
{
  m_components.push_back(std::make_tuple(&cc, &tc, &mc));
}

std::vector<std::tuple<const ColliderComponent *, TransformComponent *,
                       MovementComponent *>>::const_iterator
GridCell::begin() const
{
  return m_components.begin();
}
std::vector<std::tuple<const ColliderComponent *, TransformComponent *,
                       MovementComponent *>>::const_iterator
GridCell::end() const
{
  return m_components.end();
}

} // namespace pain
