/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file LuaScriptComponent.h
 * @brief ECS component that binds a Lua script to an entity.
 *
 * Provides a lightweight bridge between the ECS and the Lua runtime.
 * The component loads a Lua script, exposes a small API for registering
 * callbacks, and stores references to the script's lifecycle functions.
 *
 * Execution of callbacks is handled by external systems.
 */

#pragma once
#include "Core.h"
#include <SDL2/SDL_events.h>

#include "Assets/ManagerFile.h"
#include "ECS/Components/ComponentManager.h"
#include <sol/sol.hpp>

namespace pain
{

/**
 * @brief Component that attaches a Lua script to an ECS entity.
 *
 * The Lua script may register callback functions using the exposed
 * `Script` table:
 *
 * - `on_create(fn)`
 * - `on_update(fn)`
 * - `on_event(fn)`
 * - `on_render(fn)`
 * - `on_destroy(fn)`
 *
 * Each callback is optional and stored as a protected Lua function.
 * Invocation and error handling are performed by the scripting systems.
 */
struct LuaScriptComponent {
  using tag = tag::LuaScript;

  // ------------------------------------------------------------
  // Factory / Lifetime
  // ------------------------------------------------------------

  /**
   * @brief Creates a LuaScriptComponent bound to an entity.
   *
   * @param entity Entity that owns this script component.
   */
  static LuaScriptComponent create(reg::Entity entity);

  /**
   * @brief Constructs a LuaScriptComponent bound to an entity.
   *
   * @param entity Entity that owns this script component.
   */
  explicit LuaScriptComponent(reg::Entity entity);

  // ------------------------------------------------------------
  // Binding
  // ------------------------------------------------------------

  /**
   * @brief Loads and binds a Lua script to this component.
   *
   * Creates a temporary Lua table named `Script` inside the provided
   * Lua state. The script can register its callbacks through this table.
   *
   * The Lua file is loaded and executed immediately.
   *
   * @param solstate Lua state used for script execution.
   * @param scriptPath Path to the Lua script file.
   */
  void bind(sol::state &solstate, const char *scriptPath);

  // ------------------------------------------------------------
  // Script callbacks
  // ------------------------------------------------------------

  std::optional<sol::protected_function>
      m_onCreate; /**< Called once after script binding. */
  std::optional<sol::protected_function>
      m_onUpdateFunction; /**< Called every update tick. */
  std::optional<sol::protected_function>
      m_onEventFunction; /**< Called on input / engine events. */
  std::optional<sol::protected_function>
      m_onRenderFunction; /**< Called during rendering. */
  std::optional<sol::protected_function>
      m_onDestroy; /**< Called before component destruction. */

  // ------------------------------------------------------------
  // Metadata
  // ------------------------------------------------------------

  const char *m_scriptPath =
      FileManager::getDefaultLuaFile(); /**< Path to the bound Lua script. */
  reg::Entity entity;                   /**< Owning ECS entity. */
};

} // namespace pain
