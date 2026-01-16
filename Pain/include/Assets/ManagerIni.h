/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once
#include "Assets/IniWrapper.h"
#include "Assets/ManagerFile.h"
#include "Core.h"
#include "mini/ini.h"
#include "pch.h"
#include <utility>

namespace pain
{

template <typename T> struct Config {
private:
  T m_def;

public:
  inline T get() const { return value; };
  const T getDefault() const { return std::as_const(m_def); }
  T value = m_def;
  const char *name;
  Config(T t, const char *name) : m_def(t), value(m_def), name(name) {}
  Config &operator=(Config &&o)
  {
    if (this != &o) {
      m_def = o.m_def;
      name = o.name;
      value = o.value;
    }
    return *this;
  }
  Config(Config &&o) : m_def(o.m_def), value(o.value), name(o.name) {}
  NONCOPYABLE(Config);
  inline void initValue(mINI::INIStructure &ini,
                        const std::string &settingsName)
  {
    // clang-format off
    if constexpr (std::is_same_v<T, float>) {
      value = IniWrapper::getFloat(ini, settingsName, name, m_def);
    } else if constexpr (std::is_same_v<T, int>) {
      value = IniWrapper::getInteger(ini, settingsName, name, m_def);
    } else if constexpr (std::is_same_v<T, double>) {
      value = IniWrapper::getDouble(ini, settingsName, name, m_def);
    } else if constexpr (std::is_same_v<T, bool>) {
      value = IniWrapper::getBoolean(ini, settingsName, name, m_def);
    } else if constexpr (std::is_same_v<T, std::string>) {
      value = IniWrapper::get(ini, settingsName, name, m_def);
    }
    // clang-format on
  }
  inline void writeBuffer(mINI::INIStructure &ini,
                          const std::string &settingsName)
  {
    if constexpr (std::is_same_v<T, bool>) {
      ini[settingsName][name] = value ? "true" : "false";
    } else if constexpr (std::is_same_v<T, std::string>) {
      ini[settingsName][name] = value;
    } else {
      ini[settingsName][name] = std::to_string(value);
    };
  }
};
struct InternalConfig {

// # define INTERNAL_INI_CONFIGS X(type, variable, default,name)
#define INTERNAL_INI_CONFIGS                                                   \
  X(float, zoomLevel, 2.f, "initial_zoom");                                    \
  X(float, gridSize, 0.5f, "debug_grid_size");                                 \
  X(bool, swapChainTarget, true, "swap_chain_target");                         \
  X(std::string, title, "Unnamed Game", "game_title");

#define X(type, variable, default, name)                                       \
  Config<type> variable { default, name }
  INTERNAL_INI_CONFIGS
#undef X

  void readAndUpdate(const std::string &resourceFolder);
  void write(const std::string &filename);
};

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

  static bool isSettingsGuiNeeded();
  void readAndUpdate(bool isLauncher = false);
  void write(const std::string &filename);
};

} // namespace pain
