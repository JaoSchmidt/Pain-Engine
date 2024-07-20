// #include "pch.h"

// #include "ECS/Components/Movement.h"

// namespace pain
// {

// struct MovementSystem {
//   static void
//   update(const std::unordered_map<Entity, MovementComponent> &movMap,
//          std::unordered_map<Entity, TransformComponent> &traMap, double dt)
//   {
//     for (auto &pair : movMap) {
//       const MovementComponent &mc = pair.second;
//       TransformComponent &tc = traMap[pair.first];
//       const float moveAmount = (float)(dt * mc.m_translationSpeed);
//       tc.m_position = tc.m_position + mc.m_velocityDir * moveAmount;
//     }
//   }
// };
// } // namespace pain
