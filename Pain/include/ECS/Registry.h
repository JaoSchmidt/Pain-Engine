#pragma once

#include "pch.h"

#include "Core.h"

#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Components/Transform.h"
#include "ECS/Entity.h"

namespace pain
{

class Registry
{
  friend class Scene;
  template <typename T, typename... Args> T &add(int entity, Args &&...args);
  template <typename T> T &get(Entity entity);
  template <typename T> const T &get(Entity entity) const;
  template <typename T> bool has(Entity entity);
  template <typename T> void remove(Entity entity);

  template <typename T> std::unordered_map<Entity, T> &getComponentMap()
  {
    if constexpr (std::is_same_v<T, SpriteRendererComponent>) {
      return m_sprites;
    } else if constexpr (std::is_same_v<T, TransformComponent>) {
      return m_transforms;
    } else if constexpr (std::is_same_v<T, MovementComponent>) {
      return m_movement;
    } else if constexpr (std::is_same_v<T, KeyinputComponent>) {
      return m_keysinputs;
    } else {
      static_assert(std::false_type::value, "Unsupported component type");
    }
  }

  void removeAll(Entity entity)
  {
    if (has<TransformComponent>(entity))
      remove<TransformComponent>(entity);
    if (has<KeyinputComponent>(entity))
      remove<KeyinputComponent>(entity);
    if (has<SpriteRendererComponent>(entity))
      remove<SpriteRendererComponent>(entity);
    if (has<MovementComponent>(entity))
      remove<MovementComponent>(entity);
  }

  std::unordered_map<Entity, MovementComponent> m_movement;
  std::unordered_map<Entity, SpriteRendererComponent> m_sprites;
  std::unordered_map<Entity, TransformComponent> m_transforms;
  std::unordered_map<Entity, KeyinputComponent> m_keysinputs;
};

} // namespace pain
