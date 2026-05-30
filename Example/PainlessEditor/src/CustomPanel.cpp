/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// CustomPanel.cpp
#include "CustomPanel.h"
#include "CoreFiles/LogWrapper.h"
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
  if (info.m_initalSplit == 1.f) { // join as tabs in the same container
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

void CustomEditor::registerPanel(const std::string name, float split,
                                 InterfaceMenu menu)
{
  // If panel doesn't exist, create it
  if (m_customPanels.find(name) == m_customPanels.end()) {
    const std::string &panel =
        m_customPanels.emplace(name, std::vector<SubPanel>{}).first->first;
    PanelInfo &info = m_panelInfo
                          .emplace(std::piecewise_construct,
                                   std::forward_as_tuple(std::move(name)),
                                   std::forward_as_tuple(split, menu))
                          .first->second;
    if (m_dockspaceInitialized)
      dockerspaceBuild(panel, info);
  }
}

void CustomEditor::addToPanelLua(const std::string &panelName, int identifier,
                                 sol::protected_function luaFunc,
                                 sol::optional<int> optOrder)
{
  int order = optOrder.value_or(0);
  onRenderFunc wrapped = [luaFunc, this]() mutable {
    sol::protected_function_result result = luaFunc(m_imgui, m_implot);

    if (!result.valid()) {
      sol::error err = result;
      PLOG_E("Lua callback error: {}", err.what());
    }
  };

  addToPanel(panelName, identifier, wrapped, order);
}

void CustomEditor::addToPanel(const std::string &panelName, int identifier,
                              onRenderFunc callback, int order)
{
  auto it = m_customPanels.find(panelName);
  if (it == m_customPanels.end()) {
    PLOG_E("Error: no panel named {} registered");
    return;
  }

  std::vector<SubPanel> &subPanels = it->second;
  subPanels.emplace_back(callback, order, identifier);

  // std::sort(subPanels.begin(), subPanels.end());
}

void CustomEditor::buildDockerWindow()
{
  for (const auto &panel : m_customPanels)
    dockerspaceBuild(panel.first, m_panelInfo[panel.first]);
}

void CustomEditor::removeFromPanel(const std::string &panelName, int identifier)
{
  const auto pos = m_customPanels.find(panelName);
  if (pos == m_customPanels.end())
    return;

  std::vector<SubPanel> &subPanels = pos->second;
  auto it = std::find_if(
      subPanels.begin(), subPanels.end(),
      [&](const SubPanel &panel) { return panel.m_identifier == identifier; });
  if (it != subPanels.end())
    subPanels.erase(it);
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
