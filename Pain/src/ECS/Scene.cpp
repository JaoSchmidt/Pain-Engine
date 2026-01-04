// Scene.cpp
#include "ECS/Scene.h"

#include "CoreRender/RenderSys.h"
#include "CoreRender/Texture.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/Sprite.h"
#include "GUI/ImGuiSys.h"
#include "Misc/Events.h"
#include "Physics/Collision/Collider.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"
#include "Scripting/Component.h"
#include "Scripting/LuaScriptSys.h"
#include "Scripting/NativeScriptSys.h"
#include "Scripting/State.h"

#include "CoreRender/RenderSys.h"
namespace
{
struct LuaComponentDesc {
  reg::Bitmask bit;
  std::function<void(reg::Entity, reg::Bitmask)> emplace;
  std::function<void(reg::Entity)> onEmplace = nullptr;
};
} // namespace
namespace pain
{
// ------------------------------------------------
// event - lua bridge
// ------------------------------------------------

// Allow components to have logic when they are added (to an entity)
template <typename Component, reg::CompileTimeBitMaskType Manager>
void onComponentAdded(AbstractScene<Manager> &scene, reg::Entity entity)
{
  if constexpr (std::is_same_v<Component, SAPCollider>) {
    Systems::SweepAndPruneSys *s =
        scene.template getSys<Systems::SweepAndPruneSys>();
    if (s) {
      s->insertCollider(entity);
    } else {
      PLOG_W(
          "You are trying to create Sweep and Prune component without adding "
          "a proper Sweep and Prune system");
    }
  }
}

// add component to an already existing archetype
template <typename T, reg::CompileTimeBitMaskType Manager>
void pushComponentInto(reg::ArcheRegistry<Manager> &registry,
                       reg::Entity entity, reg::Bitmask bitmask, T &&t)
{
  registry.manualPush(entity, bitmask, std::move(t));
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::addEntityFunctions(const char *sceneName,
                                                sol::state &lua)
{
  // ------------------------------------------------------------
  //  Spriteless Component bind
  // ------------------------------------------------------------
  sol::table scene = lua.create_table();

  if constexpr (Manager::template isRegistered<SpritelessComponent>())
    scene.set_function(
        "Spriteless",
        sol::overload(
            [&](glm::vec2 size, sol::optional<glm::vec4> oColor) {
              return LuaComponentDesc{
                  getSingleBitmask<SpritelessComponent>(),
                  [=, this](reg::Entity e, reg::Bitmask b) {
                    if (oColor) {
                      m_registry.manualPush(
                          e, b, SpritelessComponent::createQuad(size, *oColor));
                    } else {
                      m_registry.manualPush(
                          e, b, SpritelessComponent::createQuad(size));
                    }
                  }};
            },
            [&](float radius, sol::optional<glm::vec4> oColor) {
              return LuaComponentDesc{
                  getSingleBitmask<SpritelessComponent>(),
                  [=, this](reg::Entity e, reg::Bitmask b) {
                    if (oColor) {
                      m_registry.manualPush(
                          e, b,
                          SpritelessComponent::createCircle(radius, *oColor));
                    } else {
                      m_registry.manualPush(
                          e, b, SpritelessComponent::createCircle(radius));
                    }
                  }};
            }));
  // ------------------------------------------------------------
  //  Sprite Component bind
  // ------------------------------------------------------------
  if constexpr (Manager::template isRegistered<SpriteComponent>())
    scene.set_function( //
        "Sprite",       //
        sol::overload(
            [&](const char *path, sol::optional<glm::vec2> oSize) {
              glm::vec2 size = oSize.value_or(glm::vec2{0.1f, 0.1f});
              return LuaComponentDesc{
                  getSingleBitmask<SpriteComponent>(),
                  [=, this](reg::Entity e, reg::Bitmask b) { //
                    m_registry.manualPush(e, b,
                                          SpriteComponent::create(path, size));
                  }};
            },
            [&](const char *path, unsigned short id,
                sol::optional<glm::vec2> oSize) {
              glm::vec2 size = oSize.value_or(glm::vec2{0.1f, 0.1f});
              return LuaComponentDesc{
                  getSingleBitmask<SpriteComponent>(),
                  [=, this](reg::Entity e, reg::Bitmask b) {
                    m_registry.manualPush(
                        e, b, SpriteComponent::create(path, id, size));
                  }};
            }));
  // ------------------------------------------------------------
  //  Movement2d Component bind
  // ------------------------------------------------------------
  if constexpr (Manager::template isRegistered<Movement2dComponent>())
    scene["Movement2d"] = [&](sol::optional<glm::vec2> oVel,
                              sol::optional<float> oRotationSpeed) {
      float rotationSpeed = oRotationSpeed.value_or(1.f);
      glm::vec2 vel = oVel.value_or(glm::vec2(0.f, 0.f));
      return LuaComponentDesc{
          getSingleBitmask<Movement2dComponent>(),
          [vel, rotationSpeed, this](reg::Entity e, reg::Bitmask b) {
            m_registry.manualPush(e, b,
                                  Movement2dComponent{vel, rotationSpeed});
          }};
    };
  // ------------------------------------------------------------
  //  Rotation Component bind
  // ------------------------------------------------------------
  if constexpr (Manager::template isRegistered<RotationComponent>())
    scene["Rotation"] = [&](sol::optional<float> oInitialAngle) { //
      float rot = oInitialAngle.value_or(1.f);
      return LuaComponentDesc{
          getSingleBitmask<Transform2dComponent>(),
          [rot, this](reg::Entity e, reg::Bitmask b) {
            m_registry.manualPush(e, b, RotationComponent{rot});
          } //
      };
    };

  // ------------------------------------------------------------
  //  Transform2d Component bind
  // ------------------------------------------------------------
  if constexpr (Manager::template isRegistered<Transform2dComponent>())
    scene["Transform2d"] = [&](sol::optional<glm::vec2> oPos) { //
      glm::vec2 pos = oPos.value_or(glm::vec2(0.f, 0.f));
      return LuaComponentDesc{
          getSingleBitmask<Transform2dComponent>(),
          [pos, this](reg::Entity e, reg::Bitmask b) {
            m_registry.manualPush(e, b, Transform2dComponent{pos});
          } //
      };
    };
  // ------------------------------------------------------------
  //  Sweep and Prune Component bind
  // ------------------------------------------------------------
  if constexpr (Manager::template isRegistered<SAPCollider>())
    scene.set_function(
        "SAPCollider",
        sol::overload(
            [&](glm::vec2 size, sol::optional<bool> oTrigger,
                sol::optional<glm::vec2> oOffset) {
              bool isTrigger = oTrigger.value_or(false);
              glm::vec2 offset = oOffset.value_or(glm::vec2{0.f, 0.f});

              return LuaComponentDesc{
                  getSingleBitmask<SAPCollider>(),
                  [=, this](reg::Entity e, reg::Bitmask b) {
                    m_registry.manualPush(
                        e, b, SAPCollider::createAABB(size, isTrigger, offset));
                  },
                  [this](reg::Entity e) {
                    onComponentAdded<SAPCollider>(*this, e);
                  }};
            },

            [&](float radius, sol::optional<bool> oTrigger,
                sol::optional<glm::vec2> oOffset) {
              bool isTrigger = oTrigger.value_or(false);
              glm::vec2 offset = oOffset.value_or(glm::vec2{0.f, 0.f});

              return LuaComponentDesc{
                  getSingleBitmask<SAPCollider>(),
                  [=, this](reg::Entity e, reg::Bitmask b) {
                    m_registry.manualPush(
                        e, b,
                        SAPCollider::createCircle(radius, isTrigger, offset));
                  },
                  [this](reg::Entity e) {
                    onComponentAdded<SAPCollider>(*this, e);
                  }};
            }));
  // scene.new_usertype<LuaComponentDesc>("Component", sol::no_constructor);
  scene["create_entity"] = [&](sol::table components) {
    reg::Bitmask archetype{};

    for (const auto &kv : components) {
      const auto &d = kv.second.as<LuaComponentDesc>();
      archetype |= d.bit;
    }

    reg::Entity e = m_registry.createEntity(archetype);

    for (auto &kv : components) {
      auto &d = kv.second.as<LuaComponentDesc>();
      d.emplace(e, archetype);
    }
    for (auto &kv : components) {
      auto &d = kv.second.as<LuaComponentDesc>();
      if (d.onEmplace)
        d.onEmplace(e);
    }
    return e;
  };
  lua[sceneName] = scene;
}

template <reg::CompileTimeBitMaskType Manager>
sol::state &AbstractScene<Manager>::enchanceLuaState(sol::state &state)
{
  return state;
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::emplaceLuaScript(reg::Entity entity,
                                              const char *scriptPath)
  requires(Manager::template isRegistered<LuaScriptComponent>())
{
  LuaScriptComponent &lc = getComponent<LuaScriptComponent>(entity);
  lc.bind(m_luaState, scriptPath);
  if (lc.m_onCreate) {
    sol::protected_function_result result = (*lc.m_onCreate)(lc);
    if (!result.valid()) {
      PLOG_E("Lua error (m_onUpdateFunction): {}",
             result.get<sol::error>().what());
    }
  }
}

// ------------------------------------------------

template <reg::CompileTimeBitMaskType Manager>
AbstractScene<Manager> AbstractScene<Manager>::create(reg::EventDispatcher &ed,
                                                      sol::state &solState)
{
  return AbstractScene(ed, solState);
}
template <>
AbstractScene<ComponentManager>
AbstractScene<ComponentManager>::create(reg::EventDispatcher &ed,
                                        sol::state &solState)
{
  addComponentFunctions(solState);
  return AbstractScene(ed, solState);
}
template <reg::CompileTimeBitMaskType Manager>
AbstractScene<Manager>::AbstractScene(reg::EventDispatcher &ed,
                                      sol::state &solState)
    : m_registry(), m_eventDispatcher(ed),
      m_luaState(enchanceLuaState(solState)), m_entity(createEntity()){};

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::updateSystems(DeltaTime deltaTime)
{
  PROFILE_FUNCTION();
  for (auto *sys : m_updateSystems)
    static_cast<IOnUpdate *>(sys)->onUpdate(deltaTime);
  m_eventDispatcher.update();
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::updateSystems(const SDL_Event &event)
{
  PROFILE_SCOPE("Scene::updateSystems - events for nsc");
  for (auto *sys : m_eventSystems)
    static_cast<IOnEvent *>(sys)->onEvent(event);
}

template <reg::CompileTimeBitMaskType Manager>
void AbstractScene<Manager>::renderSystems(Renderer2d &renderer,
                                           bool isMinimized,
                                           DeltaTime currentTime)
{
  for (auto *sys : m_renderSystems)
    static_cast<IOnRender *>(sys)->onRender(renderer, isMinimized, currentTime);
}

template class AbstractScene<ComponentManager>;
template class AbstractScene<UIManager>;

using Scene = AbstractScene<ComponentManager>;
using UIScene = AbstractScene<UIManager>;
} // namespace pain
