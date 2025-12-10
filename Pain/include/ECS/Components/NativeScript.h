// NativeScript.h
#pragma once

#include "Assets/DeltaTime.h"
#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Entity.h"
#include "spdlog/fmt/bundled/format.h"
#include <SDL2/SDL_events.h>

namespace pain
{
struct Renderer2d;
}

// --------------------------------------------------------------- //
// concepts for methods to use as scripts. Use this to check if a class is
// asking for a specific script function
// --------------------------------------------------------------- //

template <typename T>
concept has_onCreate_method = requires(T &&t) {
  { t.onCreate() };
};
template <typename T>
concept has_onRender_method =
    requires(T &&t, pain::Renderer2d &r, bool m, pain::DeltaTime d) {
      { t.onRender(r, m, d) };
    };
template <typename T>
concept has_onUpdate_method = requires(T &&t, pain::DeltaTime d) {
  { t.onUpdate(d) };
};
template <typename T>
concept has_onDestroy_method = requires(T &&t) {
  { t.onDestroy() };
};
template <typename T>
concept has_onEvent_method = requires(T &&t, const SDL_Event &e) {
  { t.onEvent(e) };
};

// --------------------------------------------------------------- //
// This was suggested by claude to check for privates. I.e. sometimes I forgot
// the public keyword
// &T::onFunction will be true even if the function is private
// --------------------------------------------------------------- //

template <typename T>
concept has_private_onCreate =
    requires { sizeof(&T::onCreate); } && !has_onCreate_method<T>;

template <typename T>
concept has_private_onUpdate =
    requires { sizeof(&T::onUpdate); } && !has_onUpdate_method<T>;

template <typename T>
concept has_private_onRender =
    requires { sizeof(&T::onRender); } && !has_onRender_method<T>;

template <typename T>
concept has_private_onEvent =
    requires { sizeof(&T::onEvent); } && !has_onEvent_method<T>;

template <typename T>
concept has_private_onDestroy =
    requires { sizeof(&T::onDestroy); } && !has_onDestroy_method<T>;

// --------------------------------------------------------------- //
// concepts for static asserts. Use this to check if you or other developers are
// defining incorrect functions
// --------------------------------------------------------------- //
template <typename T>
concept has_any_callable_onRender = requires(T t) { &T::onRender; };

template <typename T>
concept has_any_callable_onUpdate = requires(T t) { &T::onUpdate; };

template <typename T>
concept has_any_callable_onEvent = requires(T t) { &T::onEvent; };

template <typename T>
concept has_any_callable_onCreate = requires(T t) { &T::onCreate; };

template <typename T>
concept has_any_callable_onDestroy = requires(T t) { &T::onDestroy; };

template <typename T> void check_script_methods()
{
  // Check for wrong signatures
  if constexpr (has_any_callable_onRender<T> && !has_onRender_method<T>) {
    static_assert(false, "Error: onRender() has wrong signature! Should be "
                         "onRender(Renderer2d&, bool, DeltaTime).");
  }
  if constexpr (has_any_callable_onUpdate<T> && !has_onUpdate_method<T>) {
    static_assert(false, "Error: onUpdate() has wrong signature! Should be "
                         "onUpdate(DeltaTime).");
  }
  if constexpr (has_any_callable_onEvent<T> && !has_onEvent_method<T>) {
    static_assert(false, "Error: onEvent() has wrong signature! Should be "
                         "onEvent(const SDL_Event&).");
  }
  if constexpr (has_any_callable_onCreate<T> && !has_onCreate_method<T>) {
    static_assert(false, "Error: onCreate() has wrong signature! Should be "
                         "onCreate() with no parameters.");
  }
  if constexpr (has_any_callable_onDestroy<T> && !has_onDestroy_method<T>) {
    static_assert(false, "Error: onDestroy() has wrong signature! Should be "
                         "onDestroy() with no parameters.");
  }
  // Check for private methods
  if constexpr (has_private_onCreate<T>) {
    static_assert(false, "Error: onCreate() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (has_private_onUpdate<T>) {
    static_assert(false, "Error: onUpdate() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (has_private_onRender<T>) {
    static_assert(false, "Error: onRender() exists but is not public! Make "
                         "sure it's in the public section.");
  }
  if constexpr (has_private_onEvent<T>) {
    static_assert(false, "Error: onEvent() exists but is not public! Make sure "
                         "it's in the public section.");
  }
  if constexpr (has_private_onDestroy<T>) {
    static_assert(false, "Error: onDestroy() exists but is not public! Make "
                         "sure it's in the public section.");
  }
}

namespace pain
{
class ExtendedEntity;

// TODO: make the "setParams" function,  but perhaps in the lua script only?
// that is because the use of a variable set of parameters only make sense in
// the context of objects that need a null state. And in my current approach,
// not necesserally if any object in c++ needs to be null at the beginning. We
// can just construct them
struct NativeScriptComponent {
  ExtendedEntity *instance = nullptr;

  void (*destroyInstanceFunction)(ExtendedEntity *&) = nullptr;
  void (*onCreateFunction)(ExtendedEntity *) = nullptr;
  void (*onDestroyFunction)(ExtendedEntity *) = nullptr;
  void (*onRenderFunction)(ExtendedEntity *, Renderer2d &, bool,
                           DeltaTime) = nullptr;
  void (*onUpdateFunction)(ExtendedEntity *, DeltaTime) = nullptr;
  void (*onEventFunction)(ExtendedEntity *, const SDL_Event &) = nullptr;

  template <typename T> void bindAndInitiate(T &&t)
  {
    check_script_methods<T>();
    // static_assert(
    //     std::is_constructible_v<T, Scene &, Entity, Bitmask, Args...>,
    //     "Error: You are binding a function whose constructor doesn't
    //     implement " "ExtendedEntity constructor: (Scene&, Entity, Bitmask).
    //     Pherhaps you " "are using the defualt constructor instead of coding
    //     `using " "ExtendedEntity::ExtendedEntity;`?");
    instance = new T(std::move(t));
    // instantiateFunction = [](ExtendedEntity *&instance) { instance = new T();
    // }
    destroyInstanceFunction = [](ExtendedEntity *&instance) {
      PLOG_I("NativeScriptComponent instance {}: destructorInstanceFunction "
             "called",
             fmt::ptr(instance));
      delete static_cast<T *>(instance);
      instance = nullptr;
    };

    if constexpr (has_onCreate_method<T>) {
      onCreateFunction = [](ExtendedEntity *instance) {
        static_cast<T *>(instance)->onCreate();
      };
    } else {
      onCreateFunction = nullptr;
    }

    if constexpr (has_onDestroy_method<T>) {
      onDestroyFunction = [](ExtendedEntity *instance) {
        static_cast<T *>(instance)->onDestroy();
      };
    } else {
      onDestroyFunction = nullptr;
    }

    if constexpr (has_onRender_method<T>) {
      onRenderFunction = [](ExtendedEntity *instance, Renderer2d &renderer,
                            bool isMinimized, DeltaTime realTime) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized, realTime);
      };
    } else {
      onRenderFunction = nullptr;
    }

    // TODO: Check if has onUpdate and onEvent functions, be aware of extra
    // argument
    if constexpr (has_onUpdate_method<T>) {
      onUpdateFunction = [](ExtendedEntity *instance, DeltaTime deltaTime) {
        static_cast<T *>(instance)->onUpdate(deltaTime);
      };
    } else {
      onUpdateFunction = nullptr;
    }

    if constexpr (has_onEvent_method<T>) {
      onEventFunction = [](ExtendedEntity *instance, const SDL_Event &event) {
        static_cast<T *>(instance)->onEvent(event);
      };
    } else {
      onEventFunction = nullptr;
    }
  };
  /* Bind the script to the entity, also initialize the script instance.
   * Previously this was only "bind()" function without iniating the script
   * instance, but I just never need those two things separate, so I joined
   * them.
   */
  template <typename T, typename... Args> void bindAndInitiate(Args &&...args)
  {
    check_script_methods<T>();
    static_assert(std::is_constructible_v<T, Args...>,
                  "Error: You are binding a function whose constructor doesn't "
                  "implement ExtendedEntity constructor: (Scene&, Entity, "
                  "Bitmask). Pherhaps you are using the defualt constructor "
                  "instead of coding `using ExtendedEntity::ExtendedEntity;`?");
    instance = new T(std::forward<Args>(args)...);
    // instantiateFunction = [](ExtendedEntity *&instance) { instance = new T();
    // }
    destroyInstanceFunction = [](ExtendedEntity *&instance) {
      PLOG_I("NativeScriptComponent instance {}: destructorInstanceFunction "
             "called",
             fmt::ptr(instance));
      delete static_cast<T *>(instance);
      instance = nullptr;
    };

    if constexpr (has_onCreate_method<T>) {
      onCreateFunction = [](ExtendedEntity *instance) {
        static_cast<T *>(instance)->onCreate();
      };
    } else {
      onCreateFunction = nullptr;
    }

    if constexpr (has_onDestroy_method<T>) {
      onDestroyFunction = [](ExtendedEntity *instance) {
        static_cast<T *>(instance)->onDestroy();
      };
    } else {
      onDestroyFunction = nullptr;
    }

    if constexpr (has_onRender_method<T>) {
      onRenderFunction = [](ExtendedEntity *instance, Renderer2d &renderer,
                            bool isMinimized, DeltaTime realTime) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized, realTime);
      };
    } else {
      onRenderFunction = nullptr;
    }

    // TODO: Check if has onUpdate and onEvent functions, be aware of extra
    // argument
    if constexpr (has_onUpdate_method<T>) {
      onUpdateFunction = [](ExtendedEntity *instance, DeltaTime deltaTime) {
        static_cast<T *>(instance)->onUpdate(deltaTime);
      };
    } else {
      onUpdateFunction = nullptr;
    }

    if constexpr (has_onEvent_method<T>) {
      onEventFunction = [](ExtendedEntity *instance, const SDL_Event &event) {
        static_cast<T *>(instance)->onEvent(event);
      };
    } else {
      onEventFunction = nullptr;
    }
  };

  NativeScriptComponent() = default;
  NativeScriptComponent(const NativeScriptComponent &) = delete;
  NativeScriptComponent &operator=(const NativeScriptComponent &) = delete;
  ~NativeScriptComponent()
  {
    if (instance != nullptr) {
      destroyInstanceFunction(instance);
    } // else means this component is unbinded
  }

  // Move Assigment deals with "instance"
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
  // Move constructor deals with "instance"
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
// initialize the pointers of the Scripts functions

} // namespace pain
