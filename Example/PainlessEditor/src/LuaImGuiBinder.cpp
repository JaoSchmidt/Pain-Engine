#include "LuaImGuiBinder.h"
#include "imgui.h"
#define ImDrawIdx unsigned int
namespace painless
{
void luabinder::bindImGui(sol::state &lua, CustomEditor &editor)
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
  editor.m_imgui.set_function( //
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
  editor.m_imgui.set_function("EndChild", []() { ImGui::EndChild(); });
  editor.m_imgui.set_function(
      "PushID", [](const std::string &title) { ImGui::PushID(title.c_str()); });
  editor.m_imgui.set_function("PopID", []() { ImGui::PopID(); });
}
void luabinder::unbindCustomPanels(sol::state &lua)
{
  lua["CustomEditor"] = sol::nil;
}

} // namespace painless
