#pragma once

#include "ECS/Components/NativeScript.h"
#include "Scene.h"
namespace pain
{
// Or WorldScene
template <typename... Ts> struct TagsAllRegistered<TypeList<Ts...>> {
  static constexpr bool value = WorldComponents::allRegistered<Ts...>();
};

class Scene : public AbstractScene<WorldComponents>
{
public:
  using AbstractScene<WorldComponents>::AbstractScene;

  static Scene create(reg::EventDispatcher &eventDispatcher,
                      sol::state &solState, ThreadPool &threadPool);

  // =============================================================== //
  // NATIVE SCRIPTING RELATED
  // =============================================================== //

  template <typename S>
    requires(WorldComponents::isRegistered<NativeScriptComponent>())
  S &getNativeScript(reg::Entity entity)
  {
    NativeScriptComponent &nsc = getComponent<NativeScriptComponent>(entity);
    return static_cast<S &>(*nsc.instance);
  }

  // Move entire script class case
  template <typename N>
    requires(WorldComponents::isRegistered<NativeScriptComponent>())
  static N &emplaceScript(reg::Entity entity, Scene &scene, N &&n)
  {
    NativeScriptComponent &nsc =
        scene.getComponent<NativeScriptComponent>(entity);
    nsc.bindAndInitiate<N>(std::move(n));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<N &>(*nsc.instance);
  }

  // Emplace script inside the registry
  template <typename N, typename... Args>
    requires std::constructible_from<N, reg::Entity, Scene &, Args...> &&
             (WorldComponents::isRegistered<NativeScriptComponent>())
  static N &emplaceScript(reg::Entity entity, Scene &scene, Args &&...args)
  {
    NativeScriptComponent &nsc =
        scene.getComponent<NativeScriptComponent>(entity);
    nsc.bindAndInitiate<N>(entity, scene, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<N &>(*nsc.instance);
  }

  template <typename Sys, typename... Args>
    requires std::is_constructible_v<Sys, reg::ArcheRegistry<WorldComponents> &,
                                     reg::EventDispatcher &, Args...> &&
             std::constructible_from<Sys, reg::ArcheRegistry<WorldComponents> &,
                                     reg::EventDispatcher &, Args...> &&
             ValidSystem<Sys> && TagsAllRegistered<typename Sys::Tags>::value
  void addSystem(Args &&...args)
  {
    auto [itSystem, isInserted] = m_systems.emplace(
        std::make_pair(std::type_index(typeid(Sys)), //
                       std::make_unique<Sys>(m_registry, m_eventDispatcher,
                                             std::forward<Args>(args)...) //
                       ));
    if (!isInserted) {
      PLOG_W("Could not insert System {}", typeid(Sys).name());
      return;
    }
    Sys *s = static_cast<Sys *>(itSystem->second.get());
    if constexpr (std::derived_from<Sys, IOnEvent>)
      m_eventSystems.emplace_back(s);
    if constexpr (std::derived_from<Sys, IOnRender>)
      m_renderSystems.emplace_back(s);
    if constexpr (std::derived_from<Sys, IOnUpdate>)
      m_updateSystems.emplace_back(s);
  }
};

} // namespace pain
