#include "Physics/Collision/Collider.h"
#include "ECS/Components/Movement.h"
#include "Physics/Collision/SweepAndPruneSys.h"
namespace pain
{
Systems::SweepAndPruneSys *SAPCollider::s_systemsReference = nullptr;
}
void pain::SAPCollider::insertStaticEntity(reg::Entity entity,
                                           Transform2dComponent &tc)
{
  SAPCollider::s_systemsReference->insertStaticEntity(entity, tc, *this);
}
