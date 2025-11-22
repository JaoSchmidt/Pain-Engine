// ImGuiDebugRegistry.h
#include <functional>
#include <string>

// To be used inside ImGuiDebugMenu

namespace ImGuiDebugRegistry
{
using ImGuiFunc = std::function<void()>;

void add(const std::string &name, ImGuiFunc func, int order = 0);
void remove(const std::string &name);
void clear();
void renderAll();

}; // namespace ImGuiDebugRegistry

#ifndef NDEBUG
#if defined(__GNUC__) || defined(__clang__)
#define IMGUI_DEBUG_AUTO_NAME __PRETTY_FUNCTION__
#else
#define IMGUI_DEBUG_AUTO_NAME __FUNCTION__
#endif
/// Add log to IMGUI debug menu, automatically add the function name
#define IMGUI_PLOG(func) ::ImGuiDebugRegistry::add(IMGUI_DEBUG_AUTO_NAME, func)
/// Add log to IMGUI debug menu
#define IMGUI_PLOG_NAME(name, func) ::ImGuiDebugRegistry::add(name, func)
#else
#define IMGUI_DEBUG_ADD(func)
#endif
