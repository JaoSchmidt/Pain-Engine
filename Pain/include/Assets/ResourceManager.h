#pragma once
#include "Assets/IniWrapper.h"
#include "Core.h"
#include "mini/ini.h"
#include "pch.h"
#include <filesystem>
#include <utility>

#define SETTINGS_STRING "settings"
#define SETTINGS_FILE "config.ini"
#define INTERNAL_SETTINGS_STRING "internalSettings.ini"
#define INTERNAL_SETTINGS_FILE "internalConfig.ini"
namespace fs = std::filesystem;

namespace pain
{

// The resources manager. A simple API to concentrate all interactions with
// heavy resoruces like files while also avoiding potentials fatal errors
namespace resources
{

bool isSettingsGuiNeeded();
SDL_Surface *getSurface(const char *filepath);

const char *getDefaultLuaFile();
const std::string &getLuaScriptSource(const char *filepath);
void initiateDefaultScript();
std::string getCurrentWorkingDir();
std::string getCurrentWorkingDir(std::string append);
bool existsFile(const char *name);
bool existsFile(const std::string &name);
bool existsFile(const std::string_view &name);
bool isFile(const std::string &name);
bool isDir(const std::string &name);
bool createFile(const std::string &filename);
template <typename... P> std::string pathJoin(const P &...parts)
{
  std::filesystem::path result;
  (result /= ... /= parts);
  return result.string();
}

void clearScript();

} // namespace resources
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
  void initValue(mINI::INIStructure &ini, const std::string &settingsName)
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
  void writeBuffer(mINI::INIStructure &ini, const std::string &settingsName)
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

struct IConfig {
  virtual ~IConfig() = default;

  void readAndUpdate(const char *dirname, const char *fileName);
  virtual void readAndUpdate(const char *fileName) = 0;
};

struct InternalConfig : public IConfig {
  using IConfig::readAndUpdate;

  Config<float> zoomLevel{2.f, "InitialZoom"};
  Config<float> gridSize{0.5f, "DebugGridSize"};
  Config<std::string> title{"Unnamed Game", "GameTitle"};

  void readAndUpdate(const char *filename) override;
  void write(const char *filename);
};

struct IniConfig : public IConfig {
  using IConfig::readAndUpdate;

  Config<bool> hideConfig{false, "HideConfig"};
  Config<bool> fullscreen{false, "Fullscreen"};
  Config<int> defaultWidth{800, "DefaultWidth"};
  Config<int> defaultHeight{600, "DefaultHeight"};
  Config<std::string> assetsPath{
      pain::resources::getCurrentWorkingDir("resources"), "AssetPath"};

  void readAndUpdate(const char *filename) override;
  void write(const char *filename);
};

} // namespace pain
