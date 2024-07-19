// #pragma once
#include "pch.h"

#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Components/Movement.h"
#include "ECS/Entity.h"

namespace pain
{

struct SpriteSystem {
  static void
  render(std::unordered_map<Entity, SpriteRendererComponent> &sprMap,
         std::unordered_map<Entity, TransformComponent> &traMap)
  {
    // and finally all sprites are rendered in this render method
    // to their appropriate position on the screen
    for (auto &pair : sprMap) {
      const TransformComponent tc = traMap[pair.first];
      const SpriteRendererComponent sc = pair.second;
      Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_ptexture,
                           sc.m_tilingFactor, sc.m_color);
    }
  }
};

} // namespace pain
