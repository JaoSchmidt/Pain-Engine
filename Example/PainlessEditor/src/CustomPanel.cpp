/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// CustomPanel.cpp
#include "CustomPanel.h"
#include "CoreFiles/LogWrapper.h"
#include "imgui_internal.h"
#include <algorithm> // for std::sort
#include <sol/protected_function_result.hpp>
#include <sol/sol.hpp>
namespace painless
{

// bool comp(SubPanel &a, SubPanel &b) { return a.order > b.order; }
void CustomEditor::registerPanel(const std::string name, float split,
                                 InterfaceMenu menu)
{
  // If panel doesn't exist, create it
  if (m_customPanels.find(name) == m_customPanels.end()) {
    PLOG_I("Panel emplaced = {}", name);
    m_customPanels.emplace(name, std::vector<SubPanel>{});
    m_panelInfo.emplace(std::piecewise_construct,
                        std::forward_as_tuple(std::move(name)),
                        std::forward_as_tuple(split, menu));
  }
}

void CustomEditor::addToPanelLua(const std::string &panelName, int identifier,
                                 sol::protected_function luaFunc, int order)
{
  onRenderFunc wrapped = [&luaFunc]() mutable {
    sol::protected_function_result result = luaFunc();

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
  PLOG_I("Adding to panel {}", panelName);
  auto it = m_customPanels.find(panelName);
  if (it == m_customPanels.end()) {
    PLOG_E("Error: no panel named {} registered");
    return;
  }

  std::vector<SubPanel> &subPanels = it->second;
  subPanels.emplace_back(callback, order, identifier);

  std::sort(subPanels.begin(), subPanels.end());
}

void CustomEditor::buildDockerWindow(ImGuiID dockerSidebar,
                                     ImGuiID dockerViewport)
{
  for (auto &panel : m_customPanels) {
    PanelInfo &info = m_panelInfo[panel.first];
    switch (info.m_parentDockerspace) {
    case InterfaceMenu::BOTTOMBAR:
      ImGui::DockBuilderSplitNode(dockerViewport, ImGuiDir_Down,
                                  info.m_initalSplit, &info.m_id,
                                  &dockerViewport);
      break;
    case InterfaceMenu::SIDEBAR:
      ImGui::DockBuilderSplitNode(dockerSidebar, ImGuiDir_Down, 0.5f,
                                  &info.m_id, &dockerSidebar);
      break;
    }
    ImGui::DockBuilderDockWindow(panel.first.c_str(), info.m_id);
  }
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

void luabinder::bindToCustomPanels(sol::state &lua, CustomEditor &editor)
{
  lua.new_enum(                          //
      "InterfaceMenu",                   //
      "SIDEBAR", InterfaceMenu::SIDEBAR, //
      "BOTTOMBAR", InterfaceMenu::BOTTOMBAR);
  lua.new_usertype<painless::CustomEditor>(
      "CustomEditor",                                         //
      "add_to_panel", &painless::CustomEditor::addToPanelLua, //
      "register_panel", &painless::CustomEditor::registerPanel);
  lua["CustomEditor"] = editor;
  sol::table imgui = lua.create_named_table("ImGui");

  // --- Text ---
  imgui.set_function(
      "Text", [](const std::string &text) { ImGui::Text("%s", text.c_str()); });

  imgui.set_function("TextColored", [](float r, float g, float b, float a,
                                       const std::string &text) {
    ImGui::TextColored(ImVec4(r, g, b, a), "%s", text.c_str());
  });

  imgui.set_function("Separator", []() { ImGui::Separator(); });

  // --- Buttons ---
  imgui.set_function("Button", [](const std::string &label) {
    return ImGui::Button(label.c_str());
  });

  imgui.set_function("SmallButton", [](const std::string &label) {
    return ImGui::SmallButton(label.c_str());
  });

  // --- Layout ---
  imgui.set_function("SameLine", []() { ImGui::SameLine(); });

  imgui.set_function("Spacing", []() { ImGui::Spacing(); });

  imgui.set_function("NewLine", []() { ImGui::NewLine(); });

  // --- Inputs ---
  imgui.set_function("Checkbox", [](const std::string &label, bool value) {
    bool v = value;
    if (ImGui::Checkbox(label.c_str(), &v))
      return std::make_tuple(true, v); // changed, new value
    return std::make_tuple(false, value);
  });

  imgui.set_function("SliderFloat", [](const std::string &label, float value,
                                       float min, float max) {
    float v = value;
    if (ImGui::SliderFloat(label.c_str(), &v, min, max))
      return std::make_tuple(true, v);
    return std::make_tuple(false, value);
  });

  // --- Windows (optional if you already handle Begin/End in C++) ---
  imgui.set_function("Begin", [](const std::string &name) {
    return ImGui::Begin(name.c_str());
  });

  imgui.set_function("End", []() { ImGui::End(); });
}

} // namespace painless
