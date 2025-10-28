#pragma once

#include "Core.h"
#include "ECS/Registry/Entity.h"
#include "pch.h"
#include <unordered_set>

template <typename T>
concept has_onCollisionEnter_method = requires(T &&t, reg::Entity other) {
  { t.onCollisionEnter(other) };
};

template <typename T>
concept has_onCollisionStay_method = requires(T &&t, reg::Entity other) {
  { t.onCollisionStay(other) };
};

template <typename T>
concept has_onCollisionExit_method = requires(T &&t, reg::Entity other) {
  { t.onCollisionExit(other) };
};

namespace pain
{

class ExtendedEntity;

struct CollisionCallbackComponent {
  ExtendedEntity *instance = nullptr;

  // represent the first frame of collision
  void (*onCollisionEnterFunction)(ExtendedEntity *, reg::Entity) = nullptr;
  // Every frame while colliding
  void (*onCollisionStayFunction)(ExtendedEntity *, reg::Entity) = nullptr;
  // Frame when collision ends
  void (*onCollisionExitFunction)(ExtendedEntity *, reg::Entity) = nullptr;

  // Track previous frame collisions to detect enter/exit
  std::unordered_set<reg::Entity> m_previousCollisions;
  std::unordered_set<reg::Entity> m_currentCollisions;

  void clearCurrentFrame();
  void addCurrentCollision(reg::Entity otherId);
  bool wasCollidingLastFrame(reg::Entity otherId) const;
  bool isCollidingThisFrame(reg::Entity otherId) const;
  template <typename T> void bind(ExtendedEntity *scriptInstance)
  {
    instance = scriptInstance;
    if constexpr (has_onCollisionEnter_method<T>) {
      onCollisionEnterFunction = [](ExtendedEntity *instance,
                                    reg::Entity other) {
        static_cast<T *>(instance)->onCollisionEnter(other);
      };
    } else {
      onCollisionEnterFunction = nullptr;
    }
    if constexpr (has_onCollisionStay_method<T>) {
      onCollisionStayFunction = [](ExtendedEntity *instance,
                                   reg::Entity other) {
        static_cast<T *>(instance)->onCollisionStay(other);
      };
    } else {
      onCollisionStayFunction = nullptr;
    }
    if constexpr (has_onCollisionExit_method<T>) {
      onCollisionExitFunction = [](ExtendedEntity *instance,
                                   reg::Entity other) {
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
