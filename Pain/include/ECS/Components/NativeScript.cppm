module;
#include "CoreFiles/LogWrapper.h"
#include "spdlog/fmt/bundled/format.h"
#include <SDL2/SDL_events.h>
#include <string>

export module pain.NativeScript;
import pain.Scriptable;

// no need to export those concepts
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

export namespace pain
{

struct NativeScriptComponent {
  ScriptableEntity *instance = nullptr;
  std::string m_name = "NULL";

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
      PLOG_I("NativeScriptComponent instance {}: destructorInstanceFunction "
             "called",
             fmt::ptr(instance));
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

  NativeScriptComponent() = default;
  NativeScriptComponent(const char *name) : m_name(name) {}
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
      m_name = std::move(other.m_name);
      instantiateFunction = other.instantiateFunction;
      destroyInstanceFunction = other.destroyInstanceFunction;
      onCreateFunction = other.onCreateFunction;
      onDestroyFunction = other.onDestroyFunction;
      onRenderFunction = other.onRenderFunction;
      onUpdateFunction = other.onUpdateFunction;
      onEventFunction = other.onEventFunction;

      // Clear the other's instance
      other.instance = nullptr;
      other.instantiateFunction = nullptr;
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
    m_name = std::move(other.m_name);
    instantiateFunction = other.instantiateFunction;
    destroyInstanceFunction = other.destroyInstanceFunction;
    onCreateFunction = other.onCreateFunction;
    onDestroyFunction = other.onDestroyFunction;
    onRenderFunction = other.onRenderFunction;
    onUpdateFunction = other.onUpdateFunction;
    onEventFunction = other.onEventFunction;

    // Clear the other's instance to avoid double delete
    other.instance = nullptr;
    other.instantiateFunction = nullptr;
    other.destroyInstanceFunction = nullptr;
    other.onCreateFunction = nullptr;
    other.onDestroyFunction = nullptr;
    other.onRenderFunction = nullptr;
    other.onUpdateFunction = nullptr;
    other.onEventFunction = nullptr;
  }
};
} // namespace pain
