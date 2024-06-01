#pragma once

#include "pch.h"

#include "Core.h"

// template <typename T> struct TypeParseTraits;

/* #define REGISTER_PARSE_TYPE(X) \
//   template <> struct TypeParseTraits<X> { \
//     static const char *name; \
//   }; \
//   const char *TypeParseTraits<X>::name = #X
*/

#include "ECS/Components/Movement.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Entity.h"

namespace pain
{

class Registry
{
public:
private:
  friend class Scene;
  Registry() = default;

  template <typename T, typename... Args> T &add(Entity entity, Args &&...args);
  template <typename T> T &get(Entity entity);
  template <typename T> const T &get(Entity entity) const;
  template <typename T> bool has(Entity entity);
  template <typename T> void remove(Entity entity);

  template <typename T> std::unordered_map<Entity, T> &getComponentMap();

  // void removeAll(Entity entity)
  // {
  //   if (has<TransformComponent>(entity))
  //     remove<TransformComponent>(entity);
  //   if (has<RotationComponent>(entity))
  //     remove<RotationComponent>(entity);
  //   // if (has<SpriteRendererComponent>(entity))
  //   //   remove<SpriteRendererComponent>(entity);
  //   if (has<MovementComponent>(entity))
  //     remove<MovementComponent>(entity);
  // }

  void updateSystems(DeltaTime dt);
  void updateSystems(const SDL_Event &e);

  std::unordered_map<Entity, MovementComponent> m_movement = {};
  // std::unordered_map<Entity, SpriteRendererComponent> m_sprites;
  std::unordered_map<Entity, TransformComponent> m_transforms = {};
  std::unordered_map<Entity, RotationComponent> m_rotation = {};
};

} // namespace pain
