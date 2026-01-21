/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "GUI/ImGuiComponent.h"
#include "Scene.h"
namespace pain
{

/**
 * @brief Scene specialization dedicated to user interface logic and ImGui
 *        driven components.
 *
 * UIScene is a thin wrapper over AbstractScene using the UIComponents
 * registry configuration. It centralizes:
 *  - UI entity management.
 *  - ImGui native script binding and lifetime management.
 *  - UI-specific systems and rendering flow.
 *
 * This scene is intended to remain independent from gameplay / world logic
 * and may coexist alongside other scene types (e.g. world scenes) with
 * isolated registries and systems.
 */
class UIScene : public AbstractScene<UIComponents>
{
  template <typename... Ts> struct AreAllTagsRegistered;
  template <typename... Ts> struct AreAllTagsRegistered<TypeList<Ts...>> {
    static constexpr bool value = UIComponents::allRegistered<Ts...>();
  };

public:
  using AbstractScene<UIComponents>::AbstractScene;

  /**
   * @brief Creates a UIScene instance with the required engine services.
   *
   * @param eventDispatcher Shared engine event dispatcher.
   * @param solState Shared Lua state.
   * @param threadPool Shared thread pool.
   * @return Newly constructed UIScene.
   */
  static UIScene create(reg::EventDispatcher &eventDispatcher,
                        sol::state &solState, ThreadPool &threadPool);

  // =============================================================== //
  // IMGUI NATIVE SCRIPTING RELATED
  // =============================================================== //

  /**
   * @brief Retrieves a bound ImGui native script instance from an entity.
   *
   * The entity must own an ImGuiComponent and its instance must be of type S.
   *
   * @tparam S Expected script type.
   * @param entity Target entity.
   * @return Reference to the script instance.
   */
  template <typename S>
    requires(UIComponents::isRegistered<ImGuiComponent>())
  S &getImGuiScript(reg::Entity entity)
  {
    ImGuiComponent &nsc = getComponent<ImGuiComponent>(entity);
    return static_cast<S &>(*nsc.instance);
  }

  /**
   * @brief Binds and initializes an already constructed ImGui script instance.
   *
   * The script object is moved into the ImGuiComponent and its onCreate
   * callback is executed if available.
   *
   * @tparam N Script type.
   * @param entity Target entity.
   * @param scene Target UI scene.
   * @param n Script instance to move.
   * @return Reference to the bound script instance.
   */
  template <typename N>
    requires(UIComponents::isRegistered<ImGuiComponent>())
  static N &emplaceImGuiScript(reg::Entity entity, UIScene &scene, N &&n)
  {
    ImGuiComponent &nsc = scene.getComponent<ImGuiComponent>(entity);
    nsc.bindAndInitiate<N>(std::move(n));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<N &>(*nsc.instance);
  }

  /**
   * @brief Constructs and binds an ImGui script directly inside the component.
   *
   * The script is constructed using the provided arguments and immediately
   * bound to the entity's ImGuiComponent. The onCreate callback is executed
   * if available.
   *
   * @tparam T Script type.
   * @tparam Args Constructor argument types.
   * @param entity Target entity.
   * @param scene Target UI scene.
   * @param args Arguments forwarded to the script constructor.
   * @return Reference to the constructed script instance.
   */
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, UIScene &, Args...> &&
             (UIComponents::isRegistered<ImGuiComponent>())
  static T &emplaceImGuiScript(reg::Entity entity, UIScene &scene,
                               Args &&...args)
  {
    ImGuiComponent &nsc = scene.getComponent<ImGuiComponent>(entity);
    nsc.bindAndEmplace<T>(entity, scene, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }

  /**
   * @brief Creates and registers a new system instance.
   *
   * Systems are automatically registered into the appropriate update,
   * render and event pipelines based on their interfaces.
   *
   * @tparam Sys System type.
   * @param args Constructor arguments forwarded to the system.
   */
  template <typename Sys, typename... Args>
    requires std::is_constructible_v<Sys, reg::ArcheRegistry<UIComponents> &,
                                     reg::EventDispatcher &, Args...> &&
             std::constructible_from<Sys, reg::ArcheRegistry<UIComponents> &,
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
