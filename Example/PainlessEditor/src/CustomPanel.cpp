/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// CustomPanel.cpp
#include "CustomPanel.h"
#include "CoreFiles/LogWrapper.h"
#include "imgui_internal.h"
#include "implot.h"
#include <algorithm> // for std::sort
#include <sol/protected_function_result.hpp>
#include <sol/sol.hpp>
namespace painless
{

void CustomEditor::dockerspaceBuild(const std::string &panelName,
                                    PanelInfo &info)
{
  switch (info.m_parentDockerspace) {
  case InterfaceMenu::BOTTOMBAR:
    ImGui::DockBuilderSplitNode(m_dockerIDViewport, ImGuiDir_Down,
                                info.m_initalSplit, &info.m_id,
                                &m_dockerIDViewport);
    break;
  case InterfaceMenu::SIDEBAR:
    ImGui::DockBuilderSplitNode(m_dockerIDSidebar, ImGuiDir_Down, 0.5f,
                                &info.m_id, &m_dockerIDSidebar);
    break;
  }
  ImGui::DockBuilderDockWindow(panelName.c_str(), info.m_id);
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
  onRenderFunc wrapped = [luaFunc]() mutable {
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

void luabinder::bindToCustomPanels(sol::state &lua, CustomEditor &editor)
{
  lua.new_enum(                          //
      "InterfaceMenu",                   //
      "SIDEBAR", InterfaceMenu::SIDEBAR, //
      "BOTTOMBAR", InterfaceMenu::BOTTOMBAR);
  lua.new_usertype<painless::CustomEditor>(
      "CustomEditor",                                            //
      "add_to_panel", &painless::CustomEditor::addToPanelLua,    //
      "register_panel", &painless::CustomEditor::registerPanel); //
  lua["CustomEditor"] = &editor;
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

  imgui.set_function("GetContentRegionAvail", []() {
    ImVec2 v = ImGui::GetContentRegionAvail();
    return std::make_tuple(v.x, v.y);
  });
  imgui.set_function("GetTime", []() { return ImGui::GetTime(); });
  // --- Inputs ---
  // TODO: complete both Checkbox and SliderFloat
  // imgui.set_function("Checkbox", [](const std::string &label, bool value) {
  //   bool v = value;
  //   return ImGui::Checkbox(label.c_str(), &v);
  // });
  //
  // imgui.set_function("SliderFloat", [](const std::string &label, float value,
  //                                      float min, float max) {
  //   float v = value;
  //   if (ImGui::SliderFloat(label.c_str(), &v, min, max))
  //     return std::make_tuple(true, v);
  //   return std::make_tuple(false, value);
  // });

  // --- Optional since we already do this on each panel inside C++
  imgui.set_function("Begin", [](const std::string &name) {
    return ImGui::Begin(name.c_str());
  });

  imgui.set_function("End", []() { ImGui::End(); });
}

void luabinder::bindImPlot(sol::state &lua)
{
  sol::table implot = lua.create_named_table("ImPlot");
  implot.set_function(
      "BeginPlot", [](const std::string &title, float width, float height) {
        return ImPlot::BeginPlot(title.c_str(), ImVec2(width, height));
      });
  implot.set_function("EndPlot", []() { ImPlot::EndPlot(); });

  implot.set_function("SetupAxes",
                      [](const std::string &x, const std::string &y) {
                        ImPlot::SetupAxes(x.c_str(), y.c_str());
                      });

  implot.set_function("PlotLine", [](const std::string &label,
                                     sol::table xs_table, sol::table ys_table) {
    size_t count = std::min(xs_table.size(), ys_table.size());

    std::vector<double> xs(count);
    std::vector<double> ys(count);

    for (size_t i = 1; i <= count; ++i) {
      xs[i - 1] = xs_table.get<double>(i);
      ys[i - 1] = ys_table.get<double>(i);
    }

    ImPlot::PlotLine(label.c_str(), xs.data(), ys.data(), (int)count);
  });
}

void luabinder::unbindCustomPanels(sol::state &lua)
{
  lua["CustomEditor"] = sol::nil;
}

} // namespace painless
