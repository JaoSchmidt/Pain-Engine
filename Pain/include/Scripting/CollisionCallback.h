#pragma once

#include "Core.h"
#include "ECS/Entity.h"
#include "pch.h"
#include <unordered_set>

template <typename T>
concept has_onCollisionEnter_method = requires(T &&t, pain::Entity other) {
  { t.onCollisionEnter(other) };
};

template <typename T>
concept has_onCollisionStay_method = requires(T &&t, pain::Entity other) {
  { t.onCollisionStay(other) };
};

template <typename T>
concept has_onCollisionExit_method = requires(T &&t, pain::Entity other) {
  { t.onCollisionExit(other) };
};

namespace pain
{

class ExtendedEntity;

struct CollisionCallbackComponent {
  ExtendedEntity *instance = nullptr;

  // represent the first frame of collision
  void (*onCollisionEnterFunction)(ExtendedEntity *, Entity) = nullptr;
  // Every frame while colliding
  void (*onCollisionStayFunction)(ExtendedEntity *, Entity) = nullptr;
  // Frame when collision ends
  void (*onCollisionExitFunction)(ExtendedEntity *, Entity) = nullptr;

  // Track previous frame collisions to detect enter/exit
  std::unordered_set<Entity> m_previousCollisions;
  std::unordered_set<Entity> m_currentCollisions;

  void clearCurrentFrame();
  void addCurrentCollision(Entity otherId);
  bool wasCollidingLastFrame(Entity otherId) const;
  bool isCollidingThisFrame(Entity otherId) const;
  template <typename T> void bind(ExtendedEntity *scriptInstance)
  {
    instance = scriptInstance;
    if constexpr (has_onCollisionEnter_method<T>) {
      onCollisionEnterFunction = [](ExtendedEntity *instance, Entity other) {
        static_cast<T *>(instance)->onCollisionEnter(other);
      };
    } else {
      onCollisionEnterFunction = nullptr;
    }
    if constexpr (has_onCollisionStay_method<T>) {
      onCollisionStayFunction = [](ExtendedEntity *instance, Entity other) {
        static_cast<T *>(instance)->onCollisionStay(other);
      };
    } else {
      onCollisionStayFunction = nullptr;
    }
    if constexpr (has_onCollisionExit_method<T>) {
      onCollisionExitFunction = [](ExtendedEntity *instance, Entity other) {
        static_cast<T *>(instance)->onCollisionExit(other);
      };
    } else {
      onCollisionExitFunction = nullptr;
    }
  }

  CollisionCallbackComponent();
  NONCOPYABLE(CollisionCallbackComponent);

  CollisionCallbackComponent(CollisionCallbackComponent &&other) noexcept;
  CollisionCallbackComponent &
  operator=(CollisionCallbackComponent &&other) noexcept;
};

} // namespace pain
