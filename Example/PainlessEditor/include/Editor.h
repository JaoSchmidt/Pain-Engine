/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include <imgui.h>
#include <pain.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DebuggingMenu.h"
#include "imgui_internal.h"
namespace painless
{

class Editor : public pain::UIObject
{
public:
  static Editor &create(pain::UIScene &uiScene, pain::Application &app);
  ~Editor();
  NONCOPYABLE(Editor);
  NONMOVABLE(Editor);
  // void init(Application *app) { m_app = app; }

  void onRender(pain::RenderApi &renderAPI, 
                pain::DeltaTime dt);
  Editor(reg::Entity entity, pain::UIScene &scene, pain::Application &app);

  bool m_wasFocused = false;
  bool m_dockspaceInitialized = false;

private:
  ImGuiWindowFlags m_windowFlags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImVec2 m_avail = {200.F, 200.F};
  bool m_dockspaceOpen = true;
  pain::Application &m_app;
  painless::ImGuiDebugMenu::Script m_imGuiDebugMenu;
  float m_splitRatio = 0.7F;
  // TEMP BOTTOM BA
};

} // namespace painless
