/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// DebuggingImGui.cpp
#include "DebuggingMenu.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "ImGuiComponent.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include <string>

namespace painless
{

reg::Entity ImGuiDebugMenu::create(pain::UIScene &scene)
{
  reg::Entity entity = scene.createEntity();
  scene.createComponents(entity, painless::ImGuiComponent{});
  return entity;
}

ImGuiDebugMenu::Script::Script()
{
  // Default result file name: profile-<datetime>.json
  m_resultFileName = generateTimestampedFilename("profile-", ".json");
}

void ImGuiDebugMenu::Script::onRender(pain::Renderers &renderers,
                                      bool isMinimized,
                                      pain::DeltaTime currentTime)
{
  UNUSED(renderers)
  UNUSED(isMinimized)
  UNUSED(currentTime)

#ifdef PROFILING
  ImGui::SeparatorText("Debug Info");
  const std::string fps = "FPS: " + std::to_string(m_currentTPS);
  ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", fps.c_str());
  ImGui::InputText("Profile Base Name", &m_baseProfileName);
  ImGui::InputInt("Frame Count", &m_displayedCount);
  ImGui::TextColored(ImVec4(1, 1, 0, 1),
                     "Note: 0 = run profiler indefinitely until stopped");
  ImGui::InputText("Result File Name", &m_resultFileName);

  // buttons and logic
  if (m_frameCount > 0) {
    m_frameCount--;
    if (m_frameCount == 0)
      PROFILE_CLOSE();
  }
  if (ImGui::Button("Begin Profile")) {
    m_frameCount = m_displayedCount;
    std::string availableResultFileName =
        getNextAvailableFileName(m_resultFileName);
    ELOG_T("Starting profile on file {} with {} frames",
           availableResultFileName.c_str(), m_displayedCount);
    PROFILE_OPEN(m_baseProfileName.c_str(), availableResultFileName.c_str());
  }
  if (m_frameCount == 0) {
    if (ImGui::Button("Stop Profile")) {
      PROFILE_CLOSE();
      m_frameCount = -1; // Use -1 to indicate "not running"
    }
  } else {
    ImGui::BeginDisabled();
    ImGui::Button("Stop Profile");
    ImGui::EndDisabled();
  }

  ImGui::Separator();
#endif
}

std::string ImGuiDebugMenu::Script::generateTimestampedFilename(
    const std::string &prefix, const std::string &extension)
{
  UNUSED(extension)
  std::ostringstream oss;
  auto now = std::chrono::system_clock::now();
  auto in_time = std::chrono::system_clock::to_time_t(now);
  oss << prefix << std::put_time(std::localtime(&in_time), "%Y-%m-%d_%H-%M-%S");
  return oss.str();
}

std::string
ImGuiDebugMenu::Script::getNextAvailableFileName(const std::string &baseStem)
{
  namespace fs = std::filesystem;

  std::string extension = ".json";
  int counter = 1;
  std::string name;

  do {
    name = baseStem + ".n" + std::to_string(counter++) + extension;
  } while (fs::exists(name));

  return name;
}
void ImGuiDebugMenu::Script::onDestroy() {}

} // namespace painless
