#pragma once

#include "pch.h"

#include "Core.h"

template <typename T> struct TypeParseTraits;

#define REGISTER_PARSE_TYPE(X)                                                 \
  template <> struct TypeParseTraits<X> {                                      \
    static const char *name;                                                   \
  };                                                                           \
  const char *TypeParseTraits<X>::name = #X

#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
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

  template <typename T>
  constexpr std::unordered_map<Entity, T> &getComponentMap()
  {
    if constexpr (std::is_same_v<T, SpriteRendererComponent>) {
      return m_sprites;
    } else if constexpr (std::is_same_v<T, TransformComponent>) {
      return m_transforms;
    } else if constexpr (std::is_same_v<T, MovementComponent>) {
      return m_movement;
    } else if constexpr (std::is_same_v<T, RotationComponent>) {
      return m_rotation;
    } else {
      // static_assert(std::false_type::value, "Unsupported component type");
      throw ParseError(TypeParseTraits<T>::name);
    }
  }
  template <typename T> struct always_false : std::false_type {
  };

  void removeAll(Entity entity)
  {
    if (has<TransformComponent>(entity))
      remove<TransformComponent>(entity);
    if (has<RotationComponent>(entity))
      remove<RotationComponent>(entity);
    if (has<SpriteRendererComponent>(entity))
      remove<SpriteRendererComponent>(entity);
    if (has<MovementComponent>(entity))
      remove<MovementComponent>(entity);
  }

  std::unordered_map<Entity, MovementComponent> m_movement;
  std::unordered_map<Entity, SpriteRendererComponent> m_sprites;
  std::unordered_map<Entity, TransformComponent> m_transforms;
  std::unordered_map<Entity, RotationComponent> m_rotation;
};

} // namespace pain
