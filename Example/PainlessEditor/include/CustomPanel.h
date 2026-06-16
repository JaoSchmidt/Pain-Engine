/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

// CustomPanel.h
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "UILayer.h"

#include "imgui.h"
#include <functional>
#include <map>
#include <sol/sol.hpp>
#include <string>
namespace painless
{
using onRenderFunc = std::function<void()>;

struct SubPanel {
  onRenderFunc m_onRender;
  int m_order = 0;
  int m_identifier = 0;
  bool operator<(const SubPanel &other) const
  {
    return m_order < other.m_order;
  }
};

struct PanelInfo {
  float m_initalSplit;
  InterfaceMenu m_parentDockerspace; // dockerspace parent
  ImGuiID m_id = 0;
};

struct CustomEditor {
  std::map<std::string, std::vector<SubPanel>> m_customPanels;
  std::map<std::string, PanelInfo> m_panelInfo;
  bool m_dockspaceInitialized = false;
  ImGuiID m_dockerIDSidebar, m_dockerIDViewport;

  void registerPanel(const std::string &name, float split, InterfaceMenu menu);
  void addToPanel(const std::string &panelName, int identifier,
                  const onRenderFunc &callback, int order = 0);
  void addToPanelLua(const std::string &panelName, int identifier,
                     const sol::protected_function &luaFunc,
                     sol::optional<int> order);
  void removeFromPanel(const std::string &panelName, int identifier);
  void buildDockerWindow();
  void renderAll();

  sol::table m_imgui;
  sol::table m_implot;

private:
  void dockerspaceBuild(const std::string &panelName, PanelInfo &info);
};

} // namespace painless
