#include "LuaImGuiBinder.h"
#include "CustomPanel.h"
#include "EditorLogs.h"
#include "imgui.h"
#define ImDrawIdx unsigned int

namespace painless
{
void luabinder::bindImGui(sol::state &lua)
{

  lua.new_enum(                          //
      "InterfaceMenu",                   //
      "SIDEBAR", InterfaceMenu::SIDEBAR, //
      "BOTTOMBAR", InterfaceMenu::BOTTOMBAR);
  lua["CustomEditor"] = lua.create_table();
  lua["CustomEditor"]["add_to_panel"] =
      [](const std::string &panelName, const sol::protected_function &luaFunc,
         sol::optional<int> order) {
        return customPanel::addToPanelLua(panelName, luaFunc, order);
      };
  lua["CustomEditor"]["register_panel"] = [](const std::string &name,
                                             float split, InterfaceMenu menu) {
    customPanel::registerPanel(name, split, menu);
  };
  customPanel::getImGuiTable() = lua.create_named_table("ImGui");

  // --- Text ---
  customPanel::getImGuiTable().set_function(
      "Text", [](const std::string &text) { ImGui::Text("%s", text.c_str()); });

  customPanel::getImGuiTable().set_function(
      "TextColored",
      [](float r, float g, float b, float a, const std::string &text) {
        ImGui::TextColored(ImVec4(r, g, b, a), "%s", text.c_str());
      });

  customPanel::getImGuiTable().set_function("Separator",
                                            []() { ImGui::Separator(); });

  // --- Buttons ---
  customPanel::getImGuiTable().set_function(
      "Button",
      [](const std::string &label) { return ImGui::Button(label.c_str()); });

  customPanel::getImGuiTable().set_function(
      "SmallButton", [](const std::string &label) {
        return ImGui::SmallButton(label.c_str());
      });

  // --- Layout ---
  customPanel::getImGuiTable().set_function("SameLine",
                                            []() { ImGui::SameLine(); });

  customPanel::getImGuiTable().set_function("Spacing",
                                            []() { ImGui::Spacing(); });

  customPanel::getImGuiTable().set_function("NewLine",
                                            []() { ImGui::NewLine(); });

  customPanel::getImGuiTable().set_function("GetContentRegionAvail", []() {
    ImVec2 v = ImGui::GetContentRegionAvail();
    return std::make_tuple(v.x, v.y);
  });
  customPanel::getImGuiTable().set_function("GetTime",
                                            []() { return ImGui::GetTime(); });
  customPanel::getImGuiTable().set_function(
      "SliderFloat",
      [](const std::string &label, float value, float min, float max) {
        float v = value;
        bool changed = ImGui::SliderFloat(label.c_str(), &v, min, max);
        return std::make_tuple(changed, v);
      });
  customPanel::getImGuiTable().set_function(
      "InputInt", [](const std::string &label, int value) {
        int v = value;
        bool changed = ImGui::InputInt(label.c_str(), &v);
        return std::make_tuple(changed, v);
      });
  // --- Inputs ---
  customPanel::getImGuiTable().set_function( //
      "Checkbox", [](const std::string &label, bool value) {
        bool v = value;
        return ImGui::Checkbox(label.c_str(), &v);
      });
  // --- Optional since we already do this on each panel inside C++
  customPanel::getImGuiTable().set_function(
      "Begin",
      [](const std::string &name) { return ImGui::Begin(name.c_str()); });

  customPanel::getImGuiTable().set_function("End", []() { ImGui::End(); });
  customPanel::getImGuiTable().set_function( //
      "BeginChild",
      sol::overload(
          [](const std::string &title, float x, float y, bool isBorder) {
            if (isBorder)
              ImGui::BeginChild(title.c_str(), ImVec2(x, y));
            else
              ImGui::BeginChild(title.c_str(), ImVec2(x, y),
                                ImGuiChildFlags_Borders);
          },
          [](const std::string &title, int x, int y, bool isBorder) {
            if (isBorder)
              ImGui::BeginChild(title.c_str(), ImVec2(x, y));
            else
              ImGui::BeginChild(title.c_str(), ImVec2(x, y),
                                ImGuiChildFlags_Borders);
          }));
  customPanel::getImGuiTable().set_function("EndChild",
                                            []() { ImGui::EndChild(); });
  customPanel::getImGuiTable().set_function(
      "PushID", [](const std::string &title) { ImGui::PushID(title.c_str()); });
  customPanel::getImGuiTable().set_function("PopID", []() { ImGui::PopID(); });
}
void luabinder::unbindImGui(sol::state &lua)
{
  lua["CustomEditor"] = sol::nil;
  lua["ImGui"] = sol::nil;
}

} // namespace painless
