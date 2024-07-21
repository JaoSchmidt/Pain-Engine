#pragma once

#include "ECS/GameObject.h"

namespace pain
{

struct NativeScriptComponent {
  NativeScriptComponent(GameObject *go) : goInstance(go) {}
  ScriptableEntity *instance = nullptr;
  GameObject *goInstance = nullptr;

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

    onCreateFunction = [](ScriptableEntity *instance) {
      static_cast<T *>(instance)->onCreate();
    };
    onDestroyFunction = [](ScriptableEntity *instance) {
      static_cast<T *>(instance)->onDestroy();
    };
    onUpdateFunction = [](ScriptableEntity *instance, double ts) {
      static_cast<T *>(instance)->onUpdate(ts);
    };
    onEventFunction = [](ScriptableEntity *instance, const SDL_Event &e) {
      static_cast<T *>(instance)->onEvent(e);
    };
  }
};

} // namespace pain
