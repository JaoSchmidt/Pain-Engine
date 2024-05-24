#pragma once
#include "pch.h"

#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Texture.h"
#include "ECS/Components/Movement.h"
#include "ECS/Entity.h"
#include <memory>

namespace pain
{
struct SpriteRendererComponent {
  glm::vec4 m_color{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec2 m_size{0.1f, 0.1f};
  std::shared_ptr<Texture> m_texture;
  const float m_tilingFactor;

  SpriteRendererComponent(const SpriteRendererComponent &) = default;
  SpriteRendererComponent(const glm::vec4 &color, float tilingFactor)
      : m_color(color), m_tilingFactor(tilingFactor)
  {
  }
};

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
      Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_texture,
                           sc.m_tilingFactor, sc.m_color);
    }
  }
};

} // namespace pain
