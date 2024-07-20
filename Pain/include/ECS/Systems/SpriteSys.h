// #pragma once

// #include "CoreRender/Renderer/Renderer2d.h"

// namespace pain
// {

// struct SpriteSystem {
//   static void
//   // render(std::unordered_map<Entity, SpriteRendererComponent> &sprMap,
//   //        std::unordered_map<Entity, TransformComponent> &traMap)
//   render(const Scene &scene)
//   {
//     // and finally all sprites are rendered in this render method
//     // to their appropriate position on the screen
//     for (auto spIt = scene.begin<SpriteComponent>();
//          spIt != scene.end<SpriteComponent>(); ++spIt) {
//       const TransformComponent &tc =
//           scene.getComponent<TransformComponent>(spIt->first);
//       const SpriteComponent &sc = spIt->second;

//       Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_ptexture,
//                            sc.m_tilingFactor, sc.m_color);
//     }
//   }
// };

// } // namespace pain
