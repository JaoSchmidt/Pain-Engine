#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct Kinematics : public System<ComponentManager>, IOnUpdate {
  template <typename... Args>
    requires std::constructible_from<System<ComponentManager>, Args...> &&
             (ComponentManager::allRegistered<Transform2dComponent,
                                              Movement2dComponent>())
  Kinematics(Args &&...args) : System(std::forward<Args>(args)...){};
  Kinematics() = delete;
  void onUpdate(DeltaTime deltaTime) override;
};
} // namespace Systems
} // namespace pain
