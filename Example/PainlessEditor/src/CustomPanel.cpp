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
namespace painless
{
void CustomEditor::dockerspaceBuild(const std::string &panelName,
                                    PanelInfo &info)
{
  if (info.m_initalSplit == 1.F) { // join as tabs in the same container
    switch (info.m_parentDockerspace) {
    case InterfaceMenu::BOTTOMBAR:
      ImGui::DockBuilderDockWindow(panelName.c_str(), m_dockerIDViewport);
      break;
    case InterfaceMenu::SIDEBAR:
      ImGui::DockBuilderDockWindow(panelName.c_str(), m_dockerIDSidebar);
      break;
    }
  } else { // otherwise split the dockerspace
    switch (info.m_parentDockerspace) {
    case InterfaceMenu::BOTTOMBAR:
      ImGui::DockBuilderSplitNode(m_dockerIDViewport, ImGuiDir_Down,
                                  info.m_initalSplit, &info.m_id,
                                  &m_dockerIDViewport);
      break;
    case InterfaceMenu::SIDEBAR:
      ImGui::DockBuilderSplitNode(m_dockerIDSidebar, ImGuiDir_Down,
                                  info.m_initalSplit, &info.m_id,
                                  &m_dockerIDSidebar);
      break;
    }
    ImGui::DockBuilderDockWindow(panelName.c_str(), info.m_id);
  }
}

void CustomEditor::registerPanel(const std::string &name, float split,
                                 InterfaceMenu menu)
{
  // If panel doesn't exist, create it
  if (!m_customPanels.contains(name)) {
    const std::string &panel =
        m_customPanels.emplace(name, std::vector<SubPanel>{}).first->first;
    PanelInfo &info = m_panelInfo
                          .emplace(std::piecewise_construct,    //
                                   std::forward_as_tuple(name), //
                                   std::forward_as_tuple(split, menu))
                          .first->second;
    if (m_dockspaceInitialized)
      dockerspaceBuild(panel, info);
  }
}

int CustomEditor::addToPanelLua(const std::string &panelName,
                                const sol::protected_function &luaFunc,
                                sol::optional<int> optOrder)
{
  int order = optOrder.value_or(0);
  return addToPanel(
      panelName,
      [luaFunc, this]() mutable {
        sol::protected_function_result result = luaFunc(m_imgui, m_implot);

        if (!result.valid()) {
          sol::error err = result;
          ELOG_E("Lua callback error: {}", err.what());
        }
      },
      order);
}

int CustomEditor::addToPanel(const std::string &panelName,
                             onRenderFunc callback, int order)
{
  auto it = m_customPanels.find(panelName);
  if (it == m_customPanels.end()) {
    ELOG_E("Error: no panel named {} registered, perhaps you forgot to call "
           "registerPanel()?",
           panelName);
    return -1;
  }

  std::vector<SubPanel> &subPanels = it->second;
  int identifier = m_count++;
  subPanels.emplace_back(std::move(callback), order, identifier);
  std::sort(subPanels.begin(), subPanels.end());
  return identifier;
}

void CustomEditor::buildDockerWindow()
{
  for (const auto &panel : m_customPanels)
    dockerspaceBuild(panel.first, m_panelInfo[panel.first]);
}

void CustomEditor::removeFromPanel(const std::string &panelName,
                                   int &identifier)
{
  const auto pos = m_customPanels.find(panelName);
  if (pos == m_customPanels.end())
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

void CustomEditor::renderAll()
{
  for (auto &i : m_customPanels) {
    ImGui::Begin(i.first.c_str());
    for (SubPanel &panel : i.second) {
      panel.m_onRender();
    }
    ImGui::End();
  }
}
} // namespace painless
