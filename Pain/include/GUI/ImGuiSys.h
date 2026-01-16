/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
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

class ImGuiInstance
{
public:
  virtual void onImGuiUpdate() = 0;
  virtual ~ImGuiInstance() {};
};

namespace Systems
{
struct ImGuiSys : public System<UIComponents>, IOnRender, IOnEvent {
  using Tags = TypeList<ImGuiComponent>;
  // HACK: The singleton (static) approach assumes that there
  // will be only one context necessary for Dear ImGui to work.
  // Keep an eye on that in case it doesn't work.
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
  ~ImGuiSys();
  void onRender(Renderer2d &renderer, bool isMinimized,
                DeltaTime currentTime) override;
  void onEvent(const SDL_Event &e) override;

private:
  ImGuiIO *m_io = nullptr;
  bool m_showAnotherWindow;
  ImVec4 m_clearColor;
};

} // namespace Systems
} // namespace pain
