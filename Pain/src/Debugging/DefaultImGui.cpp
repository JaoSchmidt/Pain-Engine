#include "Debugging/DefaultImGui.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include <string>

namespace pain
{

EngineController::EngineController()
{
  // Default result file name: profile-<datetime>.json
  m_resultFileName = generateTimestampedFilename("profile-", ".json");
}

void EngineController::onImGuiUpdate()
{
  ImGui::Begin("Engine Controller");
  const std::string fps = "FPS: " + std::to_string(m_currentTPS);
  ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", fps.c_str());
  ImGui::InputText("Profile Base Name", &m_baseProfileName[0],
                   m_baseProfileName.size());
  ImGui::InputInt("Frame Count", &m_displayedCount);
  ImGui::TextColored(ImVec4(1, 1, 0, 1),
                     "Note: 0 = run profiler indefinitely until stopped");
  ImGui::InputText("Result File Name", &m_resultFileName[0],
                   m_resultFileName.size());

  // buttons and logic
  if (m_frameCount > 0) {
    m_frameCount--;
    if (m_frameCount == 0) {
      PROFILE_CLOSE();
    }
  }
  if (ImGui::Button("Begin Profile")) {
    m_frameCount = m_displayedCount;
    std::string availableResultFileName =
        getNextAvailableFileName(m_resultFileName);
    PLOG_T("Starting profile on file {} with {} frames",
           availableResultFileName.c_str(), m_displayedCount);
    PROFILE_OPEN(m_baseProfileName.c_str(), availableResultFileName.c_str());
  }
  if (m_frameCount == 0) {
    if (ImGui::Button("Stop Profile")) {
      PROFILE_CLOSE();
      m_frameCount = -1; // Use -1 to indicate "not running"
    }
  }
  ImGui::End();
}

std::string
EngineController::generateTimestampedFilename(const std::string &prefix,
                                              const std::string &extension)
{
  std::ostringstream oss;
  auto now = std::chrono::system_clock::now();
  auto in_time = std::chrono::system_clock::to_time_t(now);
  oss << prefix << std::put_time(std::localtime(&in_time), "%Y-%m-%d_%H-%M-%S");
  return oss.str();
}

std::string
EngineController::getNextAvailableFileName(const std::string &baseStem)
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

} // namespace pain
