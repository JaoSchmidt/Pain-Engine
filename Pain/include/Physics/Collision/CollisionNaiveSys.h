/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "Assets/DeltaTime.h"
#include "ECS/Systems.h"
#include "Physics/Collision/Collider.h"
#include "Physics/MovementComponent.h"
namespace pain
{

namespace Systems
{

/* NOTE: Trying to setup a logic that would only alert the developer when he/she
 * uses addSystem isn't working. The problem is that "query" is requesting
 * ColliderComponent even in the case where the compiler should clearly fail.
 * One method to resolve this perhaps would be altering "System" itself
 *
 * I've confirmed the following solution works:

 // declere require
 template <typename... Components, typename... ExcludeComponents>
    requires(CM::template allRegistered<Components...>())
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return m_registry.template query<Components...>(
        exclude<ExcludeComponents...>);
  }

 // DO NOT declere require
  template <typename... Components, typename... ExcludeComponents>
  inline std::vector<reg::ChunkView<Components...>>
  query(exclude_t<ExcludeComponents...> = {})
  {
    return reg::ChunkView<Components...>{};
  }

  That would require me to:
  1. remove the m_entity of ChunkView, replace with nothing since I don't want
 to count entities anymore
  2. replace every function inside system with an alternative where the return
 is just dummy and CM::AllRegistered is false
  3. Add a small check inside addSystem that, only then, will see if the
 function is valid

  Another option, would be to modify archeregistry a bit to allow bitmasks to
 work

*/

// NaiveCollisionSys
struct NaiveCollisionSys : public System<WorldComponents>, IOnUpdate {
  using Tags = TypeList<Transform2dComponent, //
                        Movement2dComponent,  //
                        ColliderComponent>;
  using System<WorldComponents>::System;
  NaiveCollisionSys() = delete;

  void onUpdate(DeltaTime dt) override;

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
