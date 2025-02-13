#pragma once

#include "ECS/Registry/Macros.h"
#include "ECS/Scriptable.h"
#include <SDL2/SDL_events.h>

template <typename T>
concept has_onCreate_method = requires(T &&t) {
  { t.onCreate() };
};

template <typename T>
concept has_onRender_method = requires(T &&t, double d) {
  { t.onRender(d) };
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

namespace pain
{

struct NativeScriptComponent {
  ScriptableEntity *instance = nullptr;
  static constexpr int componentID = bitMaskUnit(3);

  void (*instantiateFunction)(ScriptableEntity *&) = nullptr;
  void (*destroyInstanceFunction)(ScriptableEntity *&) = nullptr;

  void (*onCreateFunction)(ScriptableEntity *) = nullptr;
  void (*onDestroyFunction)(ScriptableEntity *) = nullptr;
  void (*onRenderFunction)(ScriptableEntity *, double) = nullptr;
  void (*onUpdateFunction)(ScriptableEntity *, double) = nullptr;
  void (*onEventFunction)(ScriptableEntity *, const SDL_Event &) = nullptr;

  template <typename T> void bind()
  {
    instantiateFunction = [](ScriptableEntity *&instance) {
      instance = new T();
    };
    destroyInstanceFunction = [](ScriptableEntity *&instance) {
      delete static_cast<T *>(instance);
      instance = nullptr;
    };

    if constexpr (has_onCreate_method<T>) {
      onCreateFunction = [](ScriptableEntity *instance) {
        static_cast<T *>(instance)->onCreate();
      };
    } else {
      onCreateFunction = nullptr;
    }

    if constexpr (has_onDestroy_method<T>) {
      onDestroyFunction = [](ScriptableEntity *instance) {
        static_cast<T *>(instance)->onDestroy();
      };
    } else {
      onDestroyFunction = nullptr;
    }

    if constexpr (has_onRender_method<T>) {
      onRenderFunction = [](ScriptableEntity *instance, double realTime) {
        static_cast<T *>(instance)->onRender(realTime);
      };
    } else {
      onRenderFunction = nullptr;
    }

    // TODO: Check if has onUpdate and onEvent functions, be aware of extra
    // argument
    if constexpr (has_onUpdate_method<T>) {
      onUpdateFunction = [](ScriptableEntity *instance, double deltaTime) {
        static_cast<T *>(instance)->onUpdate(deltaTime);
      };
    } else {
      onUpdateFunction = nullptr;
    }

    if constexpr (has_onEvent_method<T>) {
      onEventFunction = [](ScriptableEntity *instance, const SDL_Event &event) {
        static_cast<T *>(instance)->onEvent(event);
      };
    } else {
      onEventFunction = nullptr;
    }
  };
};

} // namespace pain
