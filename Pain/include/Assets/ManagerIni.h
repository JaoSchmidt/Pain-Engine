/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

/// @file ManagerIni.h
/// @brief INI-based configuration management for engine and application
/// settings.

#include "Assets/ManagerFile.h"
#include "Core.h"
#include "mini/ini.h"
#include "pch.h"
#include <utility>

namespace pain
{

/**
 * @brief Strongly-typed configuration value bound to an INI key.
 *
 * Config stores a default value, the current value loaded from disk,
 * and the associated INI key name. Supported types are:
 * float, int, double, bool, and std::string.
 *
 * @tparam T Value type.
 */
template <typename T> struct Config {
private:
  T m_def;

public:
  /// @brief Returns the current value.
  T get() const;
  /// @brief Returns the default value.
  const T getDefault() const;
  /// @brief Current value loaded from the INI file.
  T value;
  /// @brief INI key name.
  const char *name;
  /// @brief Constructs a configuration entry with a default value and key name.
  Config(T t, const char *name);
  Config &operator=(Config &&o);
  Config(Config &&o);
  NONCOPYABLE(Config);
  /// @brief Initializes the value from an INI structure.
  void initValue(mINI::INIStructure &ini, const std::string &settingsName);
  /// @brief Writes the current value into an INI structure.
  void writeBuffer(mINI::INIStructure &ini, const std::string &settingsName);
};

extern template struct Config<int>;
extern template struct Config<float>;
extern template struct Config<double>;
extern template struct Config<bool>;
extern template struct Config<std::string>;

/**
 * @brief Engine internal configuration loaded from internalConfig.ini.
 *
 * These settings are intended for engine behavior and debugging rather than
 * user-facing options.
 */
struct InternalConfig {

// # define INTERNAL_INI_CONFIGS X(type, variable, default,name)
#define INTERNAL_INI_CONFIGS                                                   \
  X(float, zoomLevel, 2.f, "initial_zoom");                                    \
  X(float, gridSize, 0.5f, "debug_grid_size");                                 \
  X(bool, swapChainTarget, true, "swap_chain_target");                         \
  X(bool, is3d, false, "is3d");                                                \
  X(std::string, title, "Unnamed Game", "game_title");

#define X(type, variable, default, name)                                       \
  Config<type> variable { default, name }
  INTERNAL_INI_CONFIGS
#undef X

  /// @brief Reads or creates the internal configuration file and updates
  /// values.
  void readAndUpdate(const std::string &resourceFolder);

  /// @brief Writes the current configuration to disk.
  void write(const std::string &filename);
};

/**
 * @brief User-facing application configuration loaded from config.ini.
 */
struct IniConfig {

  // clang-format off
#define INI_CONFIGS                                                            \
  X(bool, hideConfig, false, "HideConfig");                                    \
  X(bool, fullscreen, false, "Fullscreen");                                    \
  X(int, defaultWidth, 800, "DefaultWidth");                                   \
  X(int, defaultHeight, 600, "DefaultHeight");                                 \
  X(std::string, assetsPath, FileManager::getCurrentWorkingDir("resources"), "AssetPath");
  // clang-format on

#define X(type, variable, default, name)                                       \
  Config<type> variable { default, name }
  INI_CONFIGS
#undef X

  /// @brief Returns true if the settings GUI should be displayed on startup.
  static bool isSettingsGuiNeeded();

  /// @brief Reads config.ini and updates all configuration values.
  void readAndUpdate(bool isLauncher = false);

  /// @brief Writes the current configuration to disk.
  void write(const std::string &filename);
};

} // namespace pain
