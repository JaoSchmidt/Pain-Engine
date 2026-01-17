/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** ImGuiDebugRegistry.h */
#pragma once

#include "Core.h"
#include <functional>
#include <string>

namespace ImGuiDebugRegistry
{

/** Callable used for ImGui debug entries. */
using ImGuiFunc = std::function<void()>;
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
void add(const std::string &name, ImGuiFunc func, int order = 0);
/** Removes a previously registered debug function by name. */
void remove(const std::string &name);
/** Removes all registered debug entries. */
void clear();
/** Executes all registered debug functions in order. */
void renderAll();
} // namespace ImGuiDebugRegistry

// ============================================================= //
// **Debug Convenience Macros**
// ============================================================= //

#ifndef NDEBUG
/** Automatically uses the calling function name as the registry key. */
#define IMGUI_DEBUG_AUTO_NAME PAIN_FUNC_SIG
/** Adds a debug callback using the current function name. */
#define IMGUI_PLOG(func) ::ImGuiDebugRegistry::add(IMGUI_DEBUG_AUTO_NAME, func)
/** Adds a debug callback with an explicit name. */
#define IMGUI_PLOG_NAME(name, func) ::ImGuiDebugRegistry::add(name, func)
#else
/** Disabled in release builds. */
#define IMGUI_DEBUG_ADD(func)
#endif
