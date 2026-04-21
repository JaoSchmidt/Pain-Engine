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
#include <vector>
namespace painless
{
// Dumb lua vector wrapper
template <typename T> struct LuaVector {
  std::vector<T> data;
  LuaVector() = default;
  LuaVector(size_t n) : data(n) {}
  LuaVector(size_t n, const T &v) : data(n, v) {}
  size_t size() const { return data.size(); }
  bool empty() const { return data.empty(); }
  void clear() { data.clear(); }
  void push_back(const T &v) { data.push_back(v); }
  T &get(size_t i)
  {
    if (i == 0 || i > data.size())
      throw std::out_of_range("index");
    return data[i - 1];
  }
  void set(size_t i, const T &v)
  {
    if (i == 0 || i > data.size())
      throw std::out_of_range("index");
    else if (i == data.size())
      return data.push_back(v);
    data[i - 1] = v;
  }
  const T *raw() const { return data.data(); }
  // TODO: any compression loses data, so this could be done exclusevily on
  // implot. But right now, no need
  void downsample(int finalSize)
  {
    if (finalSize > data.size())
      return;
    std::vector<T> result;
    result.reserve(finalSize);
    size_t step = data.size() / finalSize;
    for (size_t i = 0; i < data.size(); i += step) {
      result.push_back(data[i]);
    }
    data = result;
  }
};
template <typename T> void bindVector(sol::state &sol, const std::string &name)
{
  sol.new_usertype<LuaVector<T>>(
      name,
      sol::constructors<                       //
          LuaVector<T>(),                      //
          LuaVector<T>(size_t),                //
          LuaVector<T>(size_t, const T &)>(),  //
      "size", &LuaVector<T>::size,             //
      "empty", &LuaVector<T>::empty,           //
      "clear", &LuaVector<T>::clear,           //
      "push_back", &LuaVector<T>::push_back,   //
      "downsample", &LuaVector<T>::downsample, //

      sol::meta_function::index,
      [](LuaVector<T> &v, size_t i) -> T & { return v.get(i); },
      sol::meta_function::new_index,
      [](LuaVector<T> &v, size_t i, const T &value) { v.set(i, value); },
      sol::meta_function::length, &LuaVector<T>::size //
  );
}

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
  editor.m_imgui = lua.create_named_table("ImGui");

  // --- Text ---
  editor.m_imgui.set_function(
      "Text", [](const std::string &text) { ImGui::Text("%s", text.c_str()); });

  editor.m_imgui.set_function(
      "TextColored",
      [](float r, float g, float b, float a, const std::string &text) {
        ImGui::TextColored(ImVec4(r, g, b, a), "%s", text.c_str());
      });

  editor.m_imgui.set_function("Separator", []() { ImGui::Separator(); });

  // --- Buttons ---
  editor.m_imgui.set_function("Button", [](const std::string &label) {
    return ImGui::Button(label.c_str());
  });

  editor.m_imgui.set_function("SmallButton", [](const std::string &label) {
    return ImGui::SmallButton(label.c_str());
  });

  // --- Layout ---
  editor.m_imgui.set_function("SameLine", []() { ImGui::SameLine(); });

  editor.m_imgui.set_function("Spacing", []() { ImGui::Spacing(); });

  editor.m_imgui.set_function("NewLine", []() { ImGui::NewLine(); });

  editor.m_imgui.set_function("GetContentRegionAvail", []() {
    ImVec2 v = ImGui::GetContentRegionAvail();
    return std::make_tuple(v.x, v.y);
  });
  editor.m_imgui.set_function("GetTime", []() { return ImGui::GetTime(); });
  editor.m_imgui.set_function(
      "SliderFloat",
      [](const std::string &label, float value, float min, float max) {
        float v = value;
        bool changed = ImGui::SliderFloat(label.c_str(), &v, min, max);
        return std::make_tuple(changed, v);
      });
  editor.m_imgui.set_function(
      "InputInt", [](const std::string &label, int value) {
        int v = value;
        bool changed = ImGui::InputInt(label.c_str(), &v);
        return std::make_tuple(changed, v);
      });
  // --- Inputs ---
  editor.m_imgui.set_function( //
      "Checkbox", [](const std::string &label, bool value) {
        bool v = value;
        return ImGui::Checkbox(label.c_str(), &v);
      });
  // --- Optional since we already do this on each panel inside C++
  editor.m_imgui.set_function("Begin", [](const std::string &name) {
    return ImGui::Begin(name.c_str());
  });

  editor.m_imgui.set_function("End", []() { ImGui::End(); });
}

void luabinder::bindImPlot(sol::state &lua, CustomEditor &editor)
{
  editor.m_implot = lua.create_named_table("ImPlot");
  editor.m_implot.set_function(
      "BeginPlot", [](const std::string &title, float width, float height) {
        return ImPlot::BeginPlot(title.c_str(), ImVec2(width, height));
      });
  editor.m_implot.set_function("EndPlot", []() { ImPlot::EndPlot(); });

  editor.m_implot.set_function("SetupAxes",
                               [](const std::string &x, const std::string &y) {
                                 ImPlot::SetupAxes(x.c_str(), y.c_str());
                               });

  editor.m_implot.set_function(
      "PlotLine",
      sol::overload(
          [](const std::string &label, sol::table xs_table,
             sol::table ys_table) {
            size_t count = std::min(xs_table.size(), ys_table.size());

            std::vector<double> xs(count);
            std::vector<double> ys(count);

            for (size_t i = 1; i <= count; ++i) {
              xs[i - 1] = xs_table.get<double>(i);
              ys[i - 1] = ys_table.get<double>(i);
            }

            ImPlot::PlotLine(label.c_str(), xs.data(), ys.data(), (int)count);
          },
          [](const std::string &label, const std::vector<double> &xs,
             const std::vector<double> &ys) {
            size_t count = std::min(xs.size(), ys.size());

            ImPlot::PlotLine(label.c_str(), xs.data(), ys.data(),
                             static_cast<int>(count));
          }));

  bindVector<double>(lua, "VecDouble");
  bindVector<float>(lua, "VecFloat");
  bindVector<int>(lua, "VecInt");
}

void luabinder::unbindCustomPanels(sol::state &lua)
{
  lua["CustomEditor"] = sol::nil;
}

} // namespace painless
