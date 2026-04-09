/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ImGuiRegistry.h"
#include "CoreFiles/LogWrapper.h"
#include "imgui.h"
#include <algorithm>
#include <functional>
#include <sol/sol.hpp>
#include <sol/state.hpp>
#include <string>

// To be used inside ImGuiDebugMenu
namespace ImGuiDebugRegistry
{

struct ImGuiItem {
  painless::InterfaceMenu menu;
  const std::string name;
  int order;
  bool operator<(const ImGuiItem &other) const
  {
    if (name != other.name)
      return name < other.name;
    return order < other.order;
  }
};

namespace
{
std::map<ImGuiItem, ImGuiFunc> m_items;
}; // namespace

void clear() { m_items.clear(); }

void add(const std::string &name, ImGuiFunc func, painless::InterfaceMenu menu,
         int order)
{
  m_items.insert_or_assign(ImGuiItem{menu, name, order}, ImGuiFunc(func));
}

void remove(const std::string &name)
{
  // NOTE: if this is a bottleneck (which should be unlikely), store a reverse
  // m_items lookup (e.g. m_reverseItems)
  for (auto it = m_items.begin(); it != m_items.end(); ++it) {
    if (it->first.name == name) {
      m_items.erase(it);
      return;
    }
  }
}

void renderAll(painless::InterfaceMenu menu)
{
  for (auto &item : m_items) {
    if (item.first.menu == menu)
      item.second();
  }
}
} // namespace ImGuiDebugRegistry

namespace luabinder
{

void bindImguiDebug(sol::state &lua)
{
  sol::table ig = lua.create_named_table("ImGui");
  ig.set_function("Text", [](const std::string &name, const std::string &text) {
    ImGuiDebugRegistry::add(name, [text]() { //
      ImGui::TextUnformatted(text.c_str());
    });
  });
  lua.script(R"(
        function IMGUI_PLOG(...)
            local info = debug.getinfo(2, "Sl")
            local name = (info.short_src or "lua") .. ":" .. (info.currentline or 0)

            local args = {...}
            local text = table.concat(args, " ")

            ImGui.Text(name, text)
        end
    )");
}

} // namespace luabinder
