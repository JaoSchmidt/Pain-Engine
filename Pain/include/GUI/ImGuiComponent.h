// ImGuiNativeScript.h
#pragma once

#include "CoreFiles/LogWrapper.h"
#include "Scripting/Concepts.h"
#include "spdlog/fmt/bundled/format.h"
#include <SDL2/SDL_events.h>

namespace pain
{

template <typename T>
concept has_imgui_onRender_method =
    requires(T &&t, pain::Renderer2d &r, bool m) {
      { t.onRender(r, m) };
    };
template <typename T>
concept has_imgui_private_onRender =
    requires { sizeof(&T::onRender); } && !has_imgui_onRender_method<T>;

class ExtendedEntity;

// TODO: make the "setParams" function,  but perhaps in the lua script only?
// that is because the use of a variable set of parameters only make sense in
// the context of objects that need a null state. And in my current approach,
// not necesserally if any object in c++ needs to be null at the beginning. We
// can just construct them
struct ImGuiComponent {
  ExtendedEntity *instance = nullptr;

  void (*destroyInstanceFunction)(ExtendedEntity *&) = nullptr;
  void (*onCreateFunction)(ExtendedEntity *) = nullptr;
  void (*onDestroyFunction)(ExtendedEntity *) = nullptr;
  void (*onRenderFunction)(ExtendedEntity *, Renderer2d &, bool) = nullptr;
  void (*onEventFunction)(ExtendedEntity *, const SDL_Event &) = nullptr;

  template <typename T> void bindAndInitiate(T &&t)
  {
    // check_script_methods<T>();
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
      PLOG_I("ImGuiComponent instance {}: destructorInstanceFunction "
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

    if constexpr (has_imgui_onRender_method<T>) {
      onRenderFunction = [](ExtendedEntity *instance, Renderer2d &renderer,
                            bool isMinimized) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized);
      };
    } else {
      onRenderFunction = nullptr;
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
    // check_script_methods<T>();
    static_assert(std::is_constructible_v<T, Args...>,
                  "Error: You are binding a function whose constructor doesn't "
                  "implement ExtendedEntity constructor: (Scene&, Entity, "
                  "Bitmask). Pherhaps you are using the defualt constructor "
                  "instead of coding `using ExtendedEntity::ExtendedEntity;`?");
    instance = new T(std::forward<Args>(args)...);
    // instantiateFunction = [](ExtendedEntity *&instance) { instance = new T();
    // }
    destroyInstanceFunction = [](ExtendedEntity *&instance) {
      PLOG_I("ImGuiComponent instance {}: destructorInstanceFunction "
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

    if constexpr (has_imgui_onRender_method<T>) {
      onRenderFunction = [](ExtendedEntity *instance, Renderer2d &renderer,
                            bool isMinimized) {
        static_cast<T *>(instance)->onRender(renderer, isMinimized);
      };
    } else {
      onRenderFunction = nullptr;
    }

    if constexpr (has_onEvent_method<T>) {
      onEventFunction = [](ExtendedEntity *instance, const SDL_Event &event) {
        static_cast<T *>(instance)->onEvent(event);
      };
    } else {
      onEventFunction = nullptr;
    }
  };

  ImGuiComponent() = default;
  ImGuiComponent(const ImGuiComponent &) = delete;
  ImGuiComponent &operator=(const ImGuiComponent &) = delete;
  ~ImGuiComponent()
  {
    if (instance != nullptr) {
      destroyInstanceFunction(instance);
    } // else means this component is unbinded
  }

  // Move Assigment deals with "instance"
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
  // Move constructor deals with "instance"
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
