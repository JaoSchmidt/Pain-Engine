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
  int m_identifier = -1; // e.g. to remove this sub panel later
  bool operator<(const SubPanel &other) const
  {
    return m_order < other.m_order;
  }
  // SubPanel(onRenderFunc render, int order, int id)
  //     : m_onRender(std::move(render)), m_order(order), m_identifier(id) {};
  // ~SubPanel() { PLOG_E("destr: sub {} order {}", m_identifier, m_order); }
  // SubPanel(const SubPanel &other)
  //     : m_onRender(other.m_onRender), m_order(other.m_order),
  //       m_identifier(other.m_identifier)
  // {
  //   PLOG_E("cp constructor: sub {} order {}", m_identifier, m_order);
  // };
  // SubPanel &operator=(const SubPanel &other)
  // {
  //   PLOG_E("cp assign: sub {} order {}", m_identifier, m_order);
  //   if (this != &other) {
  //     m_onRender = other.m_onRender;
  //     m_order = other.m_order;
  //     m_identifier = other.m_identifier;
  //   }
  //   return *this;
  // }
  // SubPanel(SubPanel &&other) noexcept
  //     : m_onRender(std::move(other.m_onRender)), m_order(other.m_order),
  //       m_identifier(other.m_identifier)
  // {
  //   PLOG_E("mv constructor: sub {} order {}", m_identifier, m_order);
  // };
  // SubPanel &operator=(SubPanel &&other) noexcept
  // {
  //   PLOG_E("mv assign: sub {} order {}", m_identifier, m_order);
  //   if (this != &other) {
  //     m_onRender = std::move(other.m_onRender);
  //     m_order = other.m_order;
  //     m_identifier = other.m_identifier;
  //   }
  //   return *this;
  // }
};

struct PanelInfo {
  float m_initalSplit;
  InterfaceMenu m_parentDockerspace; // dockerspace parent
  ImGuiID m_id = 0;
};

namespace customPanel
{
void registerPanel(const std::string &name, float split, InterfaceMenu menu);
int updateSubPanel(const std::string &panelName, int identifier,
                   onRenderFunc callback);

int addToPanel(const std::string &panelName, onRenderFunc callback,
               int order = 0);
int addToPanelLua(const std::string &panelName,
                  const sol::protected_function &luaFunc,
                  sol::optional<int> order);
void removeFromPanel(const std::string &panelName, int &identifier);
void buildDockerWindow(ImGuiID sidebarId, ImGuiID viewportId);
void renderAll();
void deleteAll();
sol::table &getImGuiTable();
sol::table &getImPlotTable();
} // namespace customPanel

} // namespace painless
