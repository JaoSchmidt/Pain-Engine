/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file NativeScript.h
 * @brief Native C++ scripting component for attaching behavior to entities.
 *
 * Defines NativeScriptComponent, which allows entities to own and execute
 * user-defined C++ script objects. Script instances are type-erased and
 * interacted with through stored function pointers to avoid virtual dispatch
 * and RTTI at runtime.
 *
 * Script types may optionally implement the following callbacks:
 *  - onCreate()
 *  - onDestroy()
 *  - onRender(Renderers&, bool, DeltaTime)
 *  - onUpdate(DeltaTime)
 *  - onEvent(const SDL_Event&)
 *
 * Presence of each callback is detected at compile time using concepts.
 */

#pragma once

#include "Assets/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Scene.h"
#include "Scripting/Concepts.h"
#include <SDL2/SDL_events.h>

namespace pain
{
// forward declare
struct Renderers;
template <typename SceneT> class GameObject;
class Scene;

/**
 * @struct NativeScriptComponent
 * @brief ECS component that owns and executes a native C++ script instance.
 *
 * A NativeScriptComponent binds a user-defined script type to an entity and
 * manages its lifetime. The script instance is stored as a type-erased pointer
 * and invoked through generated function pointers.
 *
 * Scripts are validated at compile time using scripting concepts to ensure
 * compatible method signatures.
 *
 * Typical usage:
 * @code
 * entity.addComponent<NativeScriptComponent>()
 *       .bindAndInitiate<MyScript>(constructorArgs...);
 * @endcode
 *
 * The component automatically destroys the script instance when the component
 * is destroyed or moved.
 */
struct NativeScriptComponent {
private:
  using Scriptable = GameObject<Scene>;

public:
  /// @brief ECS component tag used for compile-time registration.
  using tag = tag::NativeScript;
  /**
   * @brief Pointer to the bound script instance (type-erased).
   *
   * Lifetime is owned by the component. The instance is destroyed automatically
   * when the component is destroyed or reassigned.
   */
  Scriptable *instance = nullptr;
  /**
   * @name Script lifecycle function pointers
   * @brief Generated wrappers for optional script callbacks.
   *
   * These function pointers are initialized during binding depending on which
   * callbacks the script type implements.
   */
  ///@{
  void (*destroyInstanceFunction)(Scriptable *&) = nullptr;
  void (*onCreateFunction)(Scriptable *) = nullptr;
  void (*onDestroyFunction)(Scriptable *) = nullptr;
  void (*onRenderFunction)(Scriptable *, Renderers &, bool,
                           DeltaTime) = nullptr;
  void (*onUpdateFunction)(Scriptable *, DeltaTime) = nullptr;
  void (*onEventFunction)(Scriptable *, const SDL_Event &) = nullptr;
  ///@}

  /**
   * @brief Bind and initialize a script instance using an existing object.
   *
   * The provided object is moved into a newly allocated script instance and
   * bound to this component. Script callbacks are detected at compile time and
   * corresponding function wrappers are generated automatically.
   *
   * TODO:
   * This function both binds and initializes the script. If only binding is
   * required, a separate function must be implemented.
   *
   * @tparam T Script type.
   * @param t Script instance to move into this component.
   */
  template <typename T> void bindAndInitiate(T &&t)
  {
    checkScriptMethods<T>();

    instance = new T(std::move(t));
    destroyInstanceFunction = [](Scriptable *&instance) {
      delete static_cast<T *>(instance);
      instance = nullptr;
    };

    if constexpr (has_onCreate_method<T>) {
      onCreateFunction = [](Scriptable *instance) {
        static_cast<T *>(instance)->onCreate();
      };
    } else {
      onCreateFunction = nullptr;
    }

    if constexpr (has_onDestroy_method<T>) {
      onDestroyFunction = [](Scriptable *instance) {
        static_cast<T *>(instance)->onDestroy();
      };
    } else {
      onDestroyFunction = nullptr;
    }

    if constexpr (has_onRender_method<T>) {
      onRenderFunction = [](Scriptable *instance, Renderers &renderer,
                            bool isMinimized, DeltaTime realTime) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized, realTime);
      };
    } else {
      onRenderFunction = nullptr;
    }

    // TODO: Check if has onUpdate and onEvent functions, be aware of extra
    // argument
    if constexpr (has_onUpdate_method<T>) {
      onUpdateFunction = [](Scriptable *instance, DeltaTime deltaTime) {
        static_cast<T *>(instance)->onUpdate(deltaTime);
      };
    } else {
      onUpdateFunction = nullptr;
    }

    if constexpr (has_onEvent_method<T>) {
      onEventFunction = [](Scriptable *instance, const SDL_Event &event) {
        static_cast<T *>(instance)->onEvent(event);
      };
    } else {
      onEventFunction = nullptr;
    }
  };

  /**
   * @brief Bind and initialize a script instance by constructing it in place.
   *
   * The script instance is constructed dynamically using the provided
   * arguments. Script callbacks are detected at compile time and corresponding
   * function wrappers are generated automatically.
   *
   * @tparam T Script type.
   * @tparam Args Constructor argument types.
   * @param args Arguments forwarded to the script constructor.
   */
  template <typename T, typename... Args> void bindAndEmplace(Args &&...args)
  {
    checkScriptMethods<T>();
    static_assert(std::is_constructible_v<T, Args...>,
                  "Error: You are binding a function whose constructor doesn't "
                  "implement constructor: (Scene&, Entity, "
                  "Args...). Pherhaps you are using the defualt constructor "
                  "instead of coding `using Scriptable::SceneObject;`?");
    instance = new T(std::forward<Args>(args)...);
    destroyInstanceFunction = [](Scriptable *&instance) {
      delete static_cast<T *>(instance);
      instance = nullptr;
    };

    if constexpr (has_onCreate_method<T>) {
      onCreateFunction = [](Scriptable *instance) {
        static_cast<T *>(instance)->onCreate();
      };
    } else {
      onCreateFunction = nullptr;
    }

    if constexpr (has_onDestroy_method<T>) {
      onDestroyFunction = [](Scriptable *instance) {
        static_cast<T *>(instance)->onDestroy();
      };
    } else {
      onDestroyFunction = nullptr;
    }

    if constexpr (has_onRender_method<T>) {
      onRenderFunction = [](Scriptable *instance, Renderers &renderer,
                            bool isMinimized, DeltaTime realTime) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized, realTime);
      };
    } else {
      onRenderFunction = nullptr;
    }

    // TODO: Check if has onUpdate and onEvent functions, be aware of extra
    // argument
    if constexpr (has_onUpdate_method<T>) {
      onUpdateFunction = [](Scriptable *instance, DeltaTime deltaTime) {
        static_cast<T *>(instance)->onUpdate(deltaTime);
      };
    } else {
      onUpdateFunction = nullptr;
    }

    if constexpr (has_onEvent_method<T>) {
      onEventFunction = [](Scriptable *instance, const SDL_Event &event) {
        static_cast<T *>(instance)->onEvent(event);
      };
    } else {
      onEventFunction = nullptr;
    }
  };

  NativeScriptComponent() = default;
  NativeScriptComponent(const NativeScriptComponent &) = delete;
  NativeScriptComponent &operator=(const NativeScriptComponent &) = delete;
  /// @brief Destroys the bound script instance if present.
  ~NativeScriptComponent()
  {
    if (instance != nullptr) {
      destroyInstanceFunction(instance);
    } // else means this component is unbinded
  }

  /**
   * @brief Move assignment operator.
   *
   * Transfers ownership of the script instance and all callback bindings from
   * another component. The source component is left in a safe empty state.
   */
  NativeScriptComponent &operator=(NativeScriptComponent &&other) noexcept
  {
    if (this != &other) {
      // Clean up current instance if needed
      if (instance && destroyInstanceFunction)
        destroyInstanceFunction(instance);

      instance = other.instance;
      destroyInstanceFunction = other.destroyInstanceFunction;
      onCreateFunction = other.onCreateFunction;
      onDestroyFunction = other.onDestroyFunction;
      onRenderFunction = other.onRenderFunction;
      onUpdateFunction = other.onUpdateFunction;
      onEventFunction = other.onEventFunction;

      // Clear the other's instance
      other.instance = nullptr;
      other.destroyInstanceFunction = nullptr;
      other.onCreateFunction = nullptr;
      other.onDestroyFunction = nullptr;
      other.onRenderFunction = nullptr;
      other.onUpdateFunction = nullptr;
      other.onEventFunction = nullptr;
    }
    return *this;
  }
  /**
   * @brief Move constructor.
   *
   * Transfers ownership of the script instance and all callback bindings from
   * another component. The source component is cleared to avoid double
   * deletion.
   */
  NativeScriptComponent(NativeScriptComponent &&other) noexcept
  {
    instance = other.instance;
    destroyInstanceFunction = other.destroyInstanceFunction;
    onCreateFunction = other.onCreateFunction;
    onDestroyFunction = other.onDestroyFunction;
    onRenderFunction = other.onRenderFunction;
    onUpdateFunction = other.onUpdateFunction;
    onEventFunction = other.onEventFunction;

    // Clear the other's instance to avoid double delete
    other.instance = nullptr;
    other.destroyInstanceFunction = nullptr;
    other.onCreateFunction = nullptr;
    other.onDestroyFunction = nullptr;
    other.onRenderFunction = nullptr;
    other.onUpdateFunction = nullptr;
    other.onEventFunction = nullptr;
  }
};

} // namespace pain
