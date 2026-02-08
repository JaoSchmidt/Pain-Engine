/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file ImGuiNativeScript.h
 * @brief Native C++ scripting component for UI / ImGui scenes.
 *
 * Defines ImGuiComponent, which allows entities in a UI scene to own and
 * execute native C++ script objects. Scripts bound through this component are
 * expected to run exclusively inside the ImGui rendering context.
 *
 * This component mirrors the behavior of NativeScriptComponent, but is scoped
 * to UIScene and omits gameplay update semantics.
 *
 * Script types may optionally implement the following callbacks:
 *  - onCreate()
 *  - onDestroy()
 *  - onRender(Renderers&, bool, DeltaTime)
 *  - onEvent(const SDL_Event&)
 *
 * Presence of callbacks is detected at compile time using scripting concepts.
 */
#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Scene.h"
#include "Scripting/Concepts.h"
#include "spdlog/fmt/bundled/format.h"
#include <SDL2/SDL_events.h>

namespace pain
{
template <typename SceneT> class GameObject;
class UIScene;

/**
 * @struct ImGuiComponent
 * @brief ECS component that owns and executes a native UI script instance.
 *
 * ImGuiComponent binds a user-defined script type to an entity that belongs to
 * a UI scene. The script instance is stored using type erasure and accessed
 * through generated function pointers rather than virtual dispatch.
 *
 * Scripts are expected to perform rendering and input handling inside the
 * ImGui render context and should not contain gameplay logic.
 *
 * The component owns the lifetime of the script instance and guarantees proper
 * destruction when the component is destroyed or moved.
 *
 * Example usage:
 * @code
 * entity.addComponent<ImGuiComponent>()
 *       .bindAndInitiate<MyUIScript>(constructorArgs...);
 * @endcode
 */
struct ImGuiComponent {
private:
  using Scriptable = GameObject<UIScene>;

public:
  /// @brief ECS component tag used for compile-time registration.
  using tag = tag::ImGuiScript;
  /**
   * @brief Pointer to the bound UI script instance (type-erased).
   *
   * The component owns this instance and destroys it automatically.
   */
  Scriptable *instance = nullptr;
  /**
   * @name Script lifecycle function pointers
   * @brief Generated wrappers for optional script callbacks.
   */
  ///@{
  void (*destroyInstanceFunction)(Scriptable *&) = nullptr;
  void (*onCreateFunction)(Scriptable *) = nullptr;
  void (*onDestroyFunction)(Scriptable *) = nullptr;
  void (*onRenderFunction)(Scriptable *, Renderers &, bool,
                           DeltaTime dt) = nullptr;
  void (*onEventFunction)(Scriptable *, const SDL_Event &) = nullptr;
  ///@}

  /**
   * @brief Bind and initialize a UI script using an existing object.
   *
   * The provided script object is moved into a newly allocated instance and
   * bound to this component. Optional callbacks are detected at compile time
   * and corresponding wrappers are generated automatically.
   *
   * @tparam T Script type.
   * @param t Script instance to move into this component.
   */
  template <typename T> void bindAndInitiate(T &&t)
  {
    instance = new T(std::move(t));
    destroyInstanceFunction = [](Scriptable *&instance) {
      PLOG_I("ImGuiComponent instance {}: destructorInstanceFunction "
             "called",
             fmt::ptr(instance));
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
                            bool isMinimized, DeltaTime currentTime) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized,
                                             currentTime);
      };
    } else {
      onRenderFunction = nullptr;
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
   * @brief Bind and initialize a UI script by constructing it in place.
   *
   * The script instance is dynamically created using the provided constructor
   * arguments. Optional callbacks are detected at compile time and bound
   * automatically.
   *
   * @tparam T Script type.
   * @tparam Args Constructor argument types.
   * @param args Arguments forwarded to the script constructor.
   */
  template <typename T, typename... Args> void bindAndEmplace(Args &&...args)
  {
    // check_script_methods<T>();
    static_assert(std::is_constructible_v<T, Args...>,
                  "Error: You are binding a function whose constructor doesn't "
                  "implement Scriptable constructor: (Scene&, Entity, "
                  "Bitmask). Pherhaps you are using the defualt constructor "
                  "instead of coding `using Scriptable::Scriptable;`?");
    instance = new T(std::forward<Args>(args)...);
    destroyInstanceFunction = [](Scriptable *&instance) {
      PLOG_I("ImGuiComponent instance {}: destructorInstanceFunction "
             "called",
             fmt::ptr(instance));
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
                            bool isMinimized, DeltaTime currentTime) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized,
                                             currentTime);
      };
    } else {
      onRenderFunction = nullptr;
    }

    if constexpr (has_onEvent_method<T>) {
      onEventFunction = [](Scriptable *instance, const SDL_Event &event) {
        static_cast<T *>(instance)->onEvent(event);
      };
    } else {
      onEventFunction = nullptr;
    }
  };

  ImGuiComponent() = default;
  ImGuiComponent(const ImGuiComponent &) = delete;
  ImGuiComponent &operator=(const ImGuiComponent &) = delete;
  ///  @brief Destroys the bound script instance if present.
  ~ImGuiComponent()
  {
    if (instance != nullptr) {
      destroyInstanceFunction(instance);
    } // else means this component is unbinded
  }

  /**
   * @brief Move assignment operator.
   *
   * Transfers ownership of the script instance and callback bindings from
   * another component. The source component is reset to a safe empty state.
   */
  ImGuiComponent &operator=(ImGuiComponent &&other) noexcept
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
      onEventFunction = other.onEventFunction;

      // Clear the other's instance
      other.instance = nullptr;
      other.destroyInstanceFunction = nullptr;
      other.onCreateFunction = nullptr;
      other.onDestroyFunction = nullptr;
      other.onRenderFunction = nullptr;
      other.onEventFunction = nullptr;
    }
    return *this;
  }
  /**
   * @brief Move constructor.
   *
   * Transfers ownership of the script instance and callback bindings from
   * another component. The source component is cleared to avoid double
   * deletion.
   */
  ImGuiComponent(ImGuiComponent &&other) noexcept
  {
    instance = other.instance;
    destroyInstanceFunction = other.destroyInstanceFunction;
    onCreateFunction = other.onCreateFunction;
    onDestroyFunction = other.onDestroyFunction;
    onRenderFunction = other.onRenderFunction;
    onEventFunction = other.onEventFunction;

    // Clear the other's instance to avoid double delete
    other.instance = nullptr;
    other.destroyInstanceFunction = nullptr;
    other.onCreateFunction = nullptr;
    other.onDestroyFunction = nullptr;
    other.onRenderFunction = nullptr;
    other.onEventFunction = nullptr;
  }
};
// initialize the pointers of the Scripts functions

} // namespace pain
