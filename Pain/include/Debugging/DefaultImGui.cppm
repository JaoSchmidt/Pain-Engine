module;
#include "Core.h"
#include <memory>
#include <string>
export module pain.DefaultImGui;
import pain.ImGuiController;

// the purpose of this is to make a default ImGui controler for engine only
// processes
export namespace pain
{
class EngineController : public ImGuiInstance
{
public:
  EngineController();
  ~EngineController() = default;
  NONCOPYABLE(EngineController);
  EngineController(EngineController &&) = delete;
  EngineController &operator=(EngineController &&) = delete;

  void onImGuiUpdate() override;
  double m_currentTPS = 60.0;

private:
  std::string generateTimestampedFilename(const std::string &prefix,
                                          const std::string &extension);
  std::string getNextAvailableFileName(const std::string &baseName);
  std::string m_baseProfileName{"update"};
  std::string m_resultFileName{"result.json"};
  int m_displayedCount = 1;
  int m_frameCount = 1;
};

} // namespace pain
