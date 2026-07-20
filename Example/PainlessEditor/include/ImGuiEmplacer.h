/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "ECS/UIScene.h"
#include "ImGuiComponent.h"
namespace painless
{

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
  requires(pain::UIComponents::isRegistered<ImGuiComponent>())
S &getScript(reg::Entity entity, pain::UIScene &uiscene)
{
  ImGuiComponent &nsc = uiscene.getComponent<ImGuiComponent>(entity);
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
  requires(pain::UIComponents::isRegistered<ImGuiComponent>())
N &emplaceImGuiScript(reg::Entity entity, pain::UIScene &scene, N &&n)
{
  ImGuiComponent &nsc = scene.getComponent<ImGuiComponent>(entity);
  nsc.bindAndInitiate<N>(std::move(n));
  if (nsc.instance && nsc.onCreateFunction)
    nsc.onCreateFunction(nsc.instance.get());
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
  requires std::constructible_from<T, reg::Entity, pain::UIScene &, Args...> &&
           (pain::UIComponents::isRegistered<ImGuiComponent>())
T &emplaceImGuiScript(reg::Entity entity, pain::UIScene &scene, Args &&...args)
{
  ImGuiComponent &nsc = scene.getComponent<ImGuiComponent>(entity);
  nsc.bindAndEmplace<T>(entity, scene, std::forward<Args>(args)...);
  if (nsc.instance && nsc.onCreateFunction)
    nsc.onCreateFunction(nsc.instance.get());
  return static_cast<T &>(*nsc.instance);
}

} // namespace painless
