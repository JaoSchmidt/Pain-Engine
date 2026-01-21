/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ECS/Components/NativeScript.h"
#include "Scene.h"
namespace pain
{
/**
 * @brief Scene specialization dedicated to gameplay / world simulation.
 *
 * Scene is a specialization of AbstractScene using the WorldComponents
 * registry configuration. It manages:
 *  - World entities and gameplay components.
 *  - Native (C++) script binding and lifecycle.
 *  - World systems constrained to valid component tags.
 *
 * This scene represents the main simulation layer of the engine and is
 * intentionally isolated from UI-specific logic.
 */
class Scene : public AbstractScene<WorldComponents>
{
  template <typename... Ts> struct AreAllTagsRegistered;
  template <typename... Ts> struct AreAllTagsRegistered<TypeList<Ts...>> {
    static constexpr bool value = WorldComponents::allRegistered<Ts...>();
  };

public:
  using AbstractScene<WorldComponents>::AbstractScene;

  /**
   * @brief Creates a Scene instance with the required engine services.
   *
   * @param eventDispatcher Shared engine event dispatcher.
   * @param solState Shared Lua state.
   * @param threadPool Shared thread pool.
   * @return Newly constructed Scene.
   */
  static Scene create(reg::EventDispatcher &eventDispatcher,
                      sol::state &solState, ThreadPool &threadPool);

  // =============================================================== //
  // NATIVE SCRIPTING RELATED
  // =============================================================== //

  /**
   * @brief Retrieves a bound native script instance from an entity.
   *
   * The entity must own a NativeScriptComponent and its instance must be of
   * type S.
   *
   * @tparam S Expected script type.
   * @param entity Target entity.
   * @return Reference to the script instance.
   */
  template <typename S>
    requires(WorldComponents::isRegistered<NativeScriptComponent>())
  S &getNativeScript(reg::Entity entity)
  {
    NativeScriptComponent &nsc = getComponent<NativeScriptComponent>(entity);
    return static_cast<S &>(*nsc.instance);
  }

  /**
   * @brief Binds and initializes an already constructed native script instance.
   *
   * The script object is moved into the NativeScriptComponent and its onCreate
   * callback is executed if available.
   *
   * @tparam N Script type.
   * @param entity Target entity.
   * @param scene Target scene.
   * @param n Script instance to move.
   * @return Reference to the bound script instance.
   */
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

  /**
   * @brief Constructs and binds a native script directly inside the component.
   *
   * The script is constructed using the provided arguments and immediately
   * bound to the entity's NativeScriptComponent. The onCreate callback is
   * executed if available.
   *
   * @tparam N Script type.
   * @tparam Args Constructor argument types.
   * @param entity Target entity.
   * @param scene Target scene.
   * @param args Arguments forwarded to the script constructor.
   * @return Reference to the constructed script instance.
   */
  template <typename N, typename... Args>
    requires std::constructible_from<N, reg::Entity, Scene &, Args...> &&
             (WorldComponents::isRegistered<NativeScriptComponent>())
  static N &emplaceScript(reg::Entity entity, Scene &scene, Args &&...args)
  {
    NativeScriptComponent &nsc =
        scene.getComponent<NativeScriptComponent>(entity);
    nsc.bindAndEmplace<N>(entity, scene, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<N &>(*nsc.instance);
  }

  /**
   * @brief Registers a system into the scene with compile-time validation.
   *
   * System must:
   *  - be constructible with the scene registry and event dispatcher.
   *  - The system satisfies the ValidSystem concept.
   *  - All component tags declared by the system are registered in
   *    WorldComponents.
   *
   * If the system already exists, insertion is ignored and a warning is logged.
   *
   * @tparam Sys System type.
   * @tparam Args Constructor argument types.
   * @param args Arguments forwarded to the system constructor.
   */
  template <typename Sys, typename... Args>
    requires std::is_constructible_v<Sys, reg::ArcheRegistry<WorldComponents> &,
                                     reg::EventDispatcher &, Args...> &&
             std::constructible_from<Sys, reg::ArcheRegistry<WorldComponents> &,
                                     reg::EventDispatcher &, Args...> &&
             ValidSystem<Sys> && AreAllTagsRegistered<typename Sys::Tags>::value
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
