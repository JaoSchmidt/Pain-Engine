/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file ImGuiSys.h
 * @brief System responsible for managing the ImGui frame lifecycle and
 * rendering UI scripts.
 *
 * ImGuiSys owns and manages the main ImGui context for the application. It
 * switches into the ImGui frame context each render pass, dispatches input
 * events, and executes all ImGuiComponent scripts attached to UI entities.
 *
 * The system is backend-agnostic at the interface level and is designed so the
 * rendering and platform bindings can be replaced in the future without
 * affecting gameplay or UI code.
 *
 * Typical responsibilities:
 *  - Initialize and destroy the ImGui context.
 *  - Begin and end ImGui frames each render cycle.
 *  - Forward input events to ImGui and UI scripts.
 *  - Invoke ImGuiComponent render callbacks.
 *
 * This system operates exclusively on UIComponents.
 */

#pragma once

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

#include "imgui.h"

namespace pain
{
struct ImGuiComponent;
struct Renderer2d;

namespace Systems
{

/**
 * @class ImGuiSys
 * @brief ECS system that drives ImGui rendering and UI script execution.
 *
 * ImGuiSys is responsible for creating and managing the global ImGui context,
 * handling per-frame setup and teardown, and invoking all ImGuiComponent script
 * callbacks during rendering and event dispatch.
 *
 * The system assumes a single ImGui context is active at runtime.
 */
struct ImGuiSys : public System<UIComponents>, IOnRender, IOnEvent {
  /// @brief Component tags required by this system.
  using Tags = TypeList<ImGuiComponent>;

  // HACK: The singleton (static) approach assumes that there
  // will be only one context necessary for Dear ImGui to work.
  // Keep an eye on that in case it doesn't work.

  /**
   * @brief Construct the ImGui system and initialize the ImGui context.
   *
   * @param archetype Registry containing UI entities and components.
   * @param eventDispatcher Global event dispatcher.
   * @param context Platform rendering context handle.
   * @param window Platform window handle.
   * @param flags ImGui configuration flags used to initialize the context.
   */
  ImGuiSys(
      reg::ArcheRegistry<UIComponents> &archetype,
      reg::EventDispatcher &eventDispatcher, SDL_GLContext context,
      SDL_Window *window,
      ImGuiConfigFlags flags =
          ImGuiConfigFlags_NavEnableKeyboard  // Enable Keyboard Controls
          | ImGuiConfigFlags_NavEnableGamepad // Enable Gamepad Controls
          | ImGuiConfigFlags_DockingEnable    // Enable Docking
          | ImGuiConfigFlags_ViewportsEnable // Enable Multi-Viewport / Platform
  );

  /// @brief Destroy the ImGui system and release the ImGui context.
  ~ImGuiSys();

  /**
   * @brief Render callback executed once per frame.
   *
   * Begins a new ImGui frame, invokes render callbacks on all ImGuiComponent
   * instances, and finalizes rendering for the current frame.
   *
   * @param renderer Active renderer.
   * @param isMinimized Indicates whether the application window is minimized.
   * @param currentTime Current frame time.
   */
  void onRender(Renderers &renderer, bool isMinimized,
                DeltaTime currentTime) override;

  /**
   * @brief Event callback invoked for every incoming platform event.
   *
   * Forwards events to ImGui and propagates them to all active ImGuiComponent
   * instances.
   *
   * @param e Platform event.
   */
  void onEvent(const SDL_Event &e) override;

private:
  /// @brief Pointer to the active ImGui IO state.
  ImGuiIO *m_io = nullptr;

  bool m_showAnotherWindow;
  ImVec4 m_clearColor;
};

} // namespace Systems
} // namespace pain
