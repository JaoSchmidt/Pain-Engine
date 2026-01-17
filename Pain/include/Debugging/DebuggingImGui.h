/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * Default ImGui debug menu for engine-only tooling and diagnostics.
 *
 * This module creates an ImGui entity that:
 *  - Displays engine debug information (FPS, profiling controls).
 *  - Executes all registered callbacks from ImGuiDebugRegistry.
 *  - Provides basic profiling controls for developers.
 *
 * Intended for internal engine use only.
 */

#pragma once

#include "Core.h"
#include "ECS/Scriptable.h"
#include "GUI/ImGuiSys.h"

namespace pain::ImGuiDebugMenu
{

/** Creates the ImGui debug menu entity inside a UI scene. */
reg::Entity create(pain::UIScene &scene);

/** Script driving the default debug ImGui panel. */
class Script
{
public:
  Script();
  ~Script() = default;

  NONCOPYABLE(Script);
  Script(Script &&) = delete;
  Script &operator=(Script &&) = delete;

  /** Renders the debug menu and executes registered debug callbacks. */
  void onRender(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime);

  /** Cleans up debug registry state when destroyed. */
  void onDestroy();

  /** Current ticks-per-second (FPS) value displayed in the panel. */
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

} // namespace pain::ImGuiDebugMenu
