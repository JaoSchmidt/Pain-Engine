/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


// the purpose of this is to make a default ImGui controler for engine only
// processes

#include "Core.h"
#include "ECS/Scriptable.h"
#include "GUI/ImGuiSys.h"
namespace pain
{
namespace ImGuiDebugMenu
{
reg::Entity create(pain::UIScene &scene);

class Script
{
public:
  Script();
  ~Script() = default;
  NONCOPYABLE(Script);
  Script(Script &&) = delete;
  Script &operator=(Script &&) = delete;

  void onRender(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime);
  void onDestroy();
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

} // namespace ImGuiDebugMenu

} // namespace pain
