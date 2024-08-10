#pragma once

#include "ECS/Scriptable.h"

template <typename T>
concept has_onCreate_method = requires(T &&t) {
  {
    t.onCreate()
  };
};

// template <typename T>
// concept has_onUpdate_method = requires(T &&t) {
//   {
//     t.onUpdate()
//   };
// };

template <typename T>
concept has_onDestroy_method = requires(T &&t) {
  {
    t.onDestroy()
  };
};

namespace pain
{

struct NativeScriptComponent {
  ScriptableEntity *instance = nullptr;

  void (*instantiateFunction)(ScriptableEntity *&) = nullptr;
  void (*destroyInstanceFunction)(ScriptableEntity *&) = nullptr;

  void (*onCreateFunction)(ScriptableEntity *) = nullptr;
  void (*onDestroyFunction)(ScriptableEntity *) = nullptr;
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
      onCreateFunction = nullptr;
    }

    onUpdateFunction = [](ScriptableEntity *instance, double ts) {
      static_cast<T *>(instance)->onUpdate(ts);
    };
    onEventFunction = [](ScriptableEntity *instance, const SDL_Event &e) {
      static_cast<T *>(instance)->onEvent(e);
    };
  }
};

} // namespace pain
