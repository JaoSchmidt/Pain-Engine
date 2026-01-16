/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once
#include <algorithm>
#include <cctype>
#include <mini/ini.h>
#include <string>

// Lowercase helper for case-insensitive comparisons
inline std::string toLower(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

namespace IniWrapper
{

inline std::string get(mINI::INIStructure &ini, const std::string &section,
                       const std::string &key, const std::string &def = "")
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
  std::string value = get(ini, section, key, def ? "true" : "false");
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
  std::string value = get(ini, section, key, std::to_string(def));
  try {
    return std::stoi(value);
  } catch (...) {
    return def;
  }
}

inline float getFloat(mINI::INIStructure &ini, const std::string &section,
                      const std::string &key, float def = 0)
{
  std::string value = get(ini, section, key, std::to_string(def));
  try {
    return std::stof(value);
  } catch (...) {
    return def;
  }
}

inline double getDouble(mINI::INIStructure &ini, const std::string &section,
                        const std::string &key, double def = 0)
{
  std::string value = get(ini, section, key, std::to_string(def));
  try {
    return std::stod(value);
  } catch (...) {
    return def;
  }
}
} // namespace IniWrapper
