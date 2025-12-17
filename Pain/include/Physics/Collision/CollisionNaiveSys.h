#pragma once

#include "Assets/DeltaTime.h"
#include "ECS/Components/Movement.h"
#include "ECS/Systems.h"
namespace pain
{

namespace Systems
{
struct NaiveCollisionSys : public System<CMNaiveCollision> {
  template <typename... Args>
  NaiveCollisionSys(Args &&...args) : System(std::forward<Args>(args)...){};
  NaiveCollisionSys() = delete;
  void onUpdate(DeltaTime dt);
  // std::vector<std::tuple<const ColliderComponent *, Transform2dComponent *,
  //                        Movement2dComponent *>>
  //     m_movingObjects;
  // std::vector<std::tuple<const ColliderComponent *, Transform2dComponent *>>
  //     m_staticObjects;
private:
  void narrowPhaseCollisionStatic(Transform2dComponent &t_i,
                                  ColliderComponent &c_i,
                                  Movement2dComponent &m_i,
                                  reg::Entity entity_i,
                                  const Transform2dComponent &t_j,
                                  const ColliderComponent &c_j,
                                  reg::Entity entity_j);
  void narrowPhaseCollision(Transform2dComponent &t_i, ColliderComponent &c_i,
                            Movement2dComponent &m_i, reg::Entity entity_i,
                            Transform2dComponent &t_j, ColliderComponent &c_j,
                            Movement2dComponent &m_j, reg::Entity entity_j);
};

} // namespace Systems
} // namespace pain
