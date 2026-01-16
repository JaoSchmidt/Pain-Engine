/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "GUI/ImGuiDebugRegistry.h"
#include "CoreFiles/LogWrapper.h"
#include <algorithm>
#include <functional>
#include <string>

// To be used inside ImGuiDebugMenu
namespace ImGuiDebugRegistry
{

namespace
{
std::map<std::pair<int, std::string>, ImGuiFunc> m_items;
}; // namespace

void clear() { m_items.clear(); }

/** Add a function to be executed inside ImGuiDebugMenu
 Remember to pass variables by value when necessary, otherwise you might run
 into dangling references when this goes out of scope. E.g.:

 ```cpp
 ImGuiDebugRegistry::add("mouse_pointer", [=](){
    ImGui::Text("Mouse Pos: (%d,%d)", x, y);
 })
 ```

 This will copy the values of x and y. Rule of thumb: pass by references only
 internal members **/
void add(const std::string &name, ImGuiFunc func, int order)
{
  std::pair<int, std::string> key = std::make_pair(order, name);
  m_items.insert_or_assign(key, ImGuiFunc(func));
}

void remove(const std::string &name)
{
  // NOTE: if this is a bottleneck (which should be unlikely), store a reverse
  // m_items lookup (e.g. m_reverseItems)
  for (auto it = m_items.begin(); it != m_items.end(); ++it) {
    if (it->first.second == name) {
      m_items.erase(it);
      return;
    }
  }
}

void renderAll()
{
  for (auto &pair : m_items) {
    pair.second();
  }
}
} // namespace ImGuiDebugRegistry
