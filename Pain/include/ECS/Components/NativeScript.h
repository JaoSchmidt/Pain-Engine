#pragma once

#include "CoreFiles/LogWrapper.h"
#include "ECS/Registry/Entity.h"
#include "spdlog/fmt/bundled/format.h"
#include <SDL2/SDL_events.h>

namespace pain
{
struct Renderer2d;
}

template <typename T>
concept has_onCreate_method = requires(T &&t) {
  { t.onCreate() };
};
template <typename T>
concept has_onRender_method =
    requires(T &&t, pain::Renderer2d &r, bool m, double d) {
      { t.onRender(r, m, d) };
    };
template <typename T>
concept has_onUpdate_method = requires(T &&t, double d) {
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
template <typename T>
concept has_invalid_onRender_signature = requires(T t) {
  { t.onRender() };
};
template <typename T>
concept has_invalid_onUpdate_signature = requires(T t) {
  { t.onUpdate() };
};
template <typename T>
concept has_invalid_onEvent_signature = requires(T t) {
  { t.onEvent() };
};

template <typename T> void check_script_methods()
{
  if constexpr (has_invalid_onRender_signature<T>) {
    static_assert(!has_invalid_onRender_signature<T>,
                  "Warning: onRender() detected with no arguments! Should be "
                  "onRender(const Renderer&, bool, double).");
  }
  if constexpr (has_invalid_onUpdate_signature<T>) {
    static_assert(!has_invalid_onUpdate_signature<T>,
                  "Warning: onUpdate() detected with no arguments! Should be "
                  "onUpdate(double).");
  }
  if constexpr (has_invalid_onEvent_signature<T>) {
    static_assert(!has_invalid_onEvent_signature<T>,
                  "Warning: onEvent() detected with no arguments! Should be "
                  "onEvent(const SDL_Event&).");
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
                           double) = nullptr;
  void (*onUpdateFunction)(ExtendedEntity *, double) = nullptr;
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
                            bool isMinimized, double realTime) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized, realTime);
      };
    } else {
      onRenderFunction = nullptr;
    }

    // TODO: Check if has onUpdate and onEvent functions, be aware of extra
    // argument
    if constexpr (has_onUpdate_method<T>) {
      onUpdateFunction = [](ExtendedEntity *instance, double deltaTime) {
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
    // static_assert(
    //     std::is_constructible_v<T, Scene &, Entity, Bitmask, Args...>,
    //     "Error: You are binding a function whose constructor doesn't
    //     implement " "ExtendedEntity constructor: (Scene&, Entity, Bitmask).
    //     Pherhaps you " "are using the defualt constructor instead of coding
    //     `using " "ExtendedEntity::ExtendedEntity;`?");
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
                            bool isMinimized, double realTime) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized, realTime);
      };
    } else {
      onRenderFunction = nullptr;
    }

    // TODO: Check if has onUpdate and onEvent functions, be aware of extra
    // argument
    if constexpr (has_onUpdate_method<T>) {
      onUpdateFunction = [](ExtendedEntity *instance, double deltaTime) {
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
