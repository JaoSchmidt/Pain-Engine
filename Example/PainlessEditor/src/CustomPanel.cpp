/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// CustomPanel.cpp
#include "CustomPanel.h"
#include "CoreFiles/LogWrapper.h"
#include "EditorLogs.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <algorithm> // for std::sort
#include <sol/protected_function_result.hpp>
#include <sol/sol.hpp>
#include <vector>
namespace
{
int g_count = 0;
std::map<std::string, std::vector<painless::SubPanel>> g_customPanels = {};
std::map<std::string, painless::PanelInfo> g_panelInfo = {};
sol::table g_imgui = sol::nil;
sol::table g_implot = sol::nil;
} // namespace

namespace painless::customPanel
{

void dockerspaceBuild(const std::string &panelName, PanelInfo &info,
                      ImGuiID sidebarId, ImGuiID viewportId)
{
  if (info.m_initalSplit == 1.F) { // join as tabs in the same container
    switch (info.m_parentDockerspace) {
    case InterfaceMenu::BOTTOMBAR:
      ImGui::DockBuilderDockWindow(panelName.c_str(), viewportId);
      break;
    case InterfaceMenu::SIDEBAR:
      ImGui::DockBuilderDockWindow(panelName.c_str(), sidebarId);
      break;
    }
  } else { // otherwise split the dockerspace
    switch (info.m_parentDockerspace) {
    case InterfaceMenu::BOTTOMBAR:
      ImGui::DockBuilderSplitNode(viewportId, ImGuiDir_Down, info.m_initalSplit,
                                  &info.m_id, &viewportId);
      break;
    case InterfaceMenu::SIDEBAR:
      ImGui::DockBuilderSplitNode(sidebarId, ImGuiDir_Down, info.m_initalSplit,
                                  &info.m_id, &sidebarId);
      break;
    }
    ImGui::DockBuilderDockWindow(panelName.c_str(), info.m_id);
  }
}

void registerPanel(const std::string &name, float split, InterfaceMenu menu)
{
  if (!g_customPanels.contains(name)) {
    const std::string &panel =
        g_customPanels.emplace(name, std::vector<SubPanel>{}).first->first;
    g_panelInfo.emplace(std::piecewise_construct,    //
                        std::forward_as_tuple(name), //
                        std::forward_as_tuple(split, menu));
  }
}

int addToPanelLua(const std::string &panelName,
                  const sol::protected_function &luaFunc,
                  sol::optional<int> optOrder)
{
  int order = optOrder.value_or(0);
  return addToPanel(
      panelName,
      [luaFunc]() mutable {
        sol::protected_function_result result = luaFunc(g_imgui, g_implot);

        if (!result.valid()) {
          sol::error err = result;
          ELOG_E("Lua callback error: {}", err.what());
        }
      },
      order);
}

int addToPanel(const std::string &panelName, onRenderFunc callback, int order)
{
  auto it = g_customPanels.find(panelName);
  if (it == g_customPanels.end()) {
    ELOG_E("Error: no panel named {} registered, perhaps you forgot to call "
           "registerPanel()?",
           panelName);
    return -1;
  }

  std::vector<SubPanel> &subPanels = it->second;
  int identifier = g_count++;
  subPanels.emplace_back(std::move(callback), order, identifier);
  std::sort(subPanels.begin(), subPanels.end());
  return identifier;
}

int updateSubPanel(const std::string &panelName, int identifier,
                   onRenderFunc callback)
{
  auto it = g_customPanels.find(panelName);
  if (it == g_customPanels.end()) {
    ELOG_E("Error: no panel named {} registered, perhaps you forgot to call "
           "registerPanel()?",
           panelName);
    return -1;
  }

  std::vector<SubPanel> &subPanels = it->second;
  auto subIt = std::find_if( //
      subPanels.begin(), subPanels.end(),
      [identifier](const SubPanel &subPanel) {
        return subPanel.m_identifier == identifier;
      });

  if (subIt == subPanels.end()) {
    ELOG_E("Error: no subpanel with identifier {} found in panel {}.",
           identifier, panelName);
    return -1;
  }
  subIt->m_onRender = std::move(callback);
  return identifier;
}

void buildDockerWindow(ImGuiID sidebarId, ImGuiID viewportId)
{
  for (const auto &panel : g_customPanels)
    dockerspaceBuild(panel.first, g_panelInfo[panel.first], sidebarId,
                     viewportId);
}

void removeFromPanel(const std::string &panelName, int &identifier)
{
  const auto pos = g_customPanels.find(panelName);
  if (pos == g_customPanels.end())
    return;

  std::vector<SubPanel> &subPanels = pos->second;
  auto it = std::find_if(
      subPanels.begin(), subPanels.end(),
      [&](const SubPanel &panel) { return panel.m_identifier == identifier; });
  if (it != subPanels.end()) {
    subPanels.erase(it);
    identifier = -1;
  } else {
    ELOG_E("Could not remove subpanel with id = {}", identifier);
  }
}

void renderAll()
{
  for (auto &i : g_customPanels) {
    ImGui::Begin(i.first.c_str());
    for (SubPanel &panel : i.second) {
      panel.m_onRender();
    }
    ImGui::End();
  }
}
void deleteAll() { g_imgui = sol::nil; }

sol::table &getImGuiTable() { return g_imgui; }
sol::table &getImPlotTable() { return g_implot; }

} // namespace painless::customPanel
