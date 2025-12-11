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
};

} // namespace Systems
} // namespace pain
