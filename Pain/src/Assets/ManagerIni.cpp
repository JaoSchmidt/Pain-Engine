/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// MangerIni.cpp
#include "Assets/ManagerIni.h"
#include "Assets/ManagerFile.h"
#include "CoreFiles/LogWrapper.h"

#include <filesystem>

#define SETTINGS_STRING "settings"
#define SETTINGS_FILE "config.ini"
#define INTERNAL_SETTINGS_STRING "internalSettings"
#define INTERNAL_SETTINGS_FILE "internalConfig.ini"

namespace fs = std::filesystem;
namespace pain
{
// -----------------------------------------------------------------
// Internal Config
// -----------------------------------------------------------------

inline std::string toLower(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

inline std::string getString(mINI::INIStructure &ini,
                             const std::string &section, const std::string &key,
                             const std::string &def = "")
{
  if (ini.has(section) && ini[section].has(key))
    return ini[section][key];
  return def;
}

inline std::string getRead(mINI::INIStructure &ini, const std::string &section,
                           const std::string &key)
{
  if (ini.has(section) && ini[section].has(key))
    return ini[section][key];
  return "";
}

inline bool getBoolean(mINI::INIStructure &ini, const std::string &section,
                       const std::string &key, bool def = false)
{
  std::string value = getString(ini, section, key, def ? "true" : "false");
  value = toLower(value);

  if (value == "true" || value == "1" || value == "yes" || value == "on")
    return true;
  if (value == "false" || value == "0" || value == "no" || value == "off")
    return false;
  return def; // fallback if invalid
}

inline int getInteger(mINI::INIStructure &ini, const std::string &section,
                      const std::string &key, int def = 0)
{
  std::string value = getString(ini, section, key, std::to_string(def));
  try {
    return std::stoi(value);
  } catch (...) {
    return def;
  }
}

inline float getFloat(mINI::INIStructure &ini, const std::string &section,
                      const std::string &key, float def = 0)
{
  std::string value = getString(ini, section, key, std::to_string(def));
  try {
    return std::stof(value);
  } catch (...) {
    return def;
  }
}

inline double getDouble(mINI::INIStructure &ini, const std::string &section,
                        const std::string &key, double def = 0)
{
  std::string value = getString(ini, section, key, std::to_string(def));
  try {
    return std::stod(value);
  } catch (...) {
    return def;
  }
}

// --------------------- Config ---------------------
template <typename T> T Config<T>::get() const { return value; }

template <typename T> const T Config<T>::getDefault() const
{
  return std::as_const(m_def);
}
template <typename T>
Config<T>::Config(T t, const char *name)
    : m_def(std::move(t)), value(m_def), name(name){};
template <typename T> Config<T> &Config<T>::operator=(Config &&o)
{
  if (this != &o) {
    m_def = std::move(o.m_def);
    value = std::move(o.value);
    name = o.name;
  }
  return *this;
}
template <typename T>
Config<T>::Config(Config &&o)
    : m_def(std::move(o.m_def)), value(std::move(o.value)), name(o.name){};

template <typename T>
void Config<T>::initValue(mINI::INIStructure &ini,
                          const std::string &settingsName)
{
  if constexpr (std::is_same_v<T, float>) {
    value = getFloat(ini, settingsName, name, m_def);
  } else if constexpr (std::is_same_v<T, int>) {
    value = getInteger(ini, settingsName, name, m_def);
  } else if constexpr (std::is_same_v<T, double>) {
    value = getDouble(ini, settingsName, name, m_def);
  } else if constexpr (std::is_same_v<T, bool>) {
    value = getBoolean(ini, settingsName, name, m_def);
  } else if constexpr (std::is_same_v<T, std::string>) {
    value = getString(ini, settingsName, name, m_def);
  }
}

template <typename T>
void Config<T>::writeBuffer(mINI::INIStructure &ini,
                            const std::string &settingsName)
{
  if constexpr (std::is_same_v<T, bool>) {
    ini[settingsName][name] = value ? "true" : "false";
  } else if constexpr (std::is_same_v<T, std::string>) {
    ini[settingsName][name] = value;
  } else {
    ini[settingsName][name] = std::to_string(value);
  }
}

template struct Config<int>;
template struct Config<float>;
template struct Config<double>;
template struct Config<bool>;
template struct Config<std::string>;

// -----------------------------------------------------------------
// Internal Config
// -----------------------------------------------------------------

void InternalConfig::readAndUpdate(const std::string &resourceFolder)
{
  fs::path pathVar = resourceFolder;
  pathVar /= INTERNAL_SETTINGS_FILE;
  const std::string fullPath = pathVar.string();

  if (!FileManager::existsFile(fullPath) || !FileManager::isFile(fullPath)) {
    if (FileManager::isDir(resourceFolder)) {
      PLOG_W("{} not found, generating a new one", INTERNAL_SETTINGS_FILE);
      FileManager::createFile(fullPath);
    } else
      std::terminate();

    write(fullPath);
    return;
  }

  mINI::INIFile file(fullPath);
  mINI::INIStructure ini;
  file.read(ini);

#define X(type, variable, default, name)                                       \
  variable.initValue(ini, INTERNAL_SETTINGS_STRING)
  INTERNAL_INI_CONFIGS
#undef X
}

void InternalConfig::write(const std::string &filename)
{
  mINI::INIFile file(filename);
  mINI::INIStructure ini;

  file.read(ini);
#define X(type, variable, default, name)                                       \
  variable.writeBuffer(ini, INTERNAL_SETTINGS_STRING)
  INTERNAL_INI_CONFIGS
#undef X
  file.write(ini);
}
#undef INTERNAL_INI_CONFIGS

// -----------------------------------------------------------------
// Ini Config
// -----------------------------------------------------------------

bool IniConfig::isSettingsGuiNeeded()
{
  if (!FileManager::existsFile(SETTINGS_FILE))
    return true;

  mINI::INIFile file(SETTINGS_FILE);
  mINI::INIStructure ini;

  if (!file.read(ini))
    return true;

  return !getBoolean(ini, "settings", "HideConfig", true);
}

void IniConfig::readAndUpdate(bool isLauncher)
{
  if (!isLauncher && !FileManager::existsFile(SETTINGS_FILE)) {
    PLOG_W("config.ini file should exist at the beginning! Did you skip the "
           "launcher?");
    FileManager::createFile(SETTINGS_FILE);
    write(SETTINGS_FILE); // this create config.ini with default values
    return;
  }

  mINI::INIFile file(SETTINGS_FILE);
  mINI::INIStructure ini;
  file.read(ini);

#define X(type, variable, default, name)                                       \
  variable.initValue(ini, SETTINGS_STRING)
  INI_CONFIGS
#undef X

  FileManager::setDefaultPath(assetsPath.value.c_str());
}

void IniConfig::write(const std::string &filename)
{
  mINI::INIFile file(filename);
  mINI::INIStructure ini;

  file.read(ini);
#define X(type, variable, default, name)                                       \
  variable.writeBuffer(ini, SETTINGS_STRING)
  INI_CONFIGS
#undef X
  file.write(ini);
}

} // namespace pain
