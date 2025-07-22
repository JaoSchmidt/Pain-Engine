#include "CoreFiles/ResourceManagerSing.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Texture.h"
#include <cstdio>
#include <utility>

namespace pain
{
bool m_enableDefaultWarning = false;

// NOTE: remember folks, surfaceMap is in the static/global memory but its
// content are in the heap
static std::map<const char *, Texture *> m_textureMap = {};
static std::map<const char *, sol::load_result> m_luaFileMap = {};
static std::map<const char *, std::string> m_luaScriptSource;
inline bool exists_file(const std::string &name)
{
  struct stat buffer;
  return (stat(name.c_str(), &buffer) == 0);
}

// load a script inside filepath, if it's not already loaded, created
const std::string &resources::getLuaScriptSource(const char *filepath)
{
  auto it = m_luaScriptSource.find(filepath);
  if (it != m_luaScriptSource.end())
    return it->second;

  if (!exists_file(filepath)) {
    PLOG_E("Could not locate filepath {}", filepath);
    auto it = m_luaScriptSource.find(getDefaultLuaFile());
    P_ASSERT(it != m_luaScriptSource.end(),
             "Could not load default script file {}", getDefaultLuaFile());
    return it->second;
  }

  std::ifstream in(filepath);
  std::string line;
  std::stringstream ss;

  while (getline(in, line)) {
    ss << line << '\n';
  }

  m_luaScriptSource.emplace(filepath, ss.str());
  return m_luaScriptSource[filepath];
}
Texture &resources::createDumpTexture(const char *name, uint32_t width,
                                      uint32_t height, ImageFormat imf)
{
  auto search = m_textureMap.find(name);
  if (search != m_textureMap.end()) {
    PLOG_W("You are trying to create {}, but it already was created", name);
    return *(search->second);
  }
  m_textureMap[name] = new Texture(width, height, imf);
  return *m_textureMap[name];
}

void resources::initiateDefaultValues(sol::state &solstate)
{
  Texture &whiteTexture = resources::createDumpTexture("BLANK", 1, 1);
  const uint32_t whiteTextureData = 0xffffffff;
  whiteTexture.setData(&whiteTextureData, sizeof(uint32_t));
  getTexture("resources/default/textures/defaultTexture.png");
  getLuaScriptSource(getDefaultLuaFile());
  m_enableDefaultWarning = true;
}

const char *resources::getDefaultLuaFile()
{
  return "resources/default/scripts/default.lua";
}

Texture &resources::getDefaultTexture(resources::DEFAULT_TEXTURE defTex)
{
  P_ASSERT(m_textureMap.contains("BLANK"),
           "Some default values are missing, did you remember to call "
           "initateDefaultValues?")
  if (m_enableDefaultWarning)
    PLOG_W("Using a default texture {}", static_cast<int>(defTex));
  switch (defTex) {
  case GENERAL:
    return *m_textureMap.at("resources/default/textures/defaultTexture.png");
  case ERROR:
    return *m_textureMap.at("resources/default/textures/defaultTexture.png");
  case BLANK:
    return *m_textureMap.at("BLANK");
  }
}

Texture &resources::getTexture(const char *filepathOrName)
{
  auto search = m_textureMap.find(filepathOrName);
  if (search != m_textureMap.end()) {
    return *(search->second);
  }
  Texture *texture = new Texture(filepathOrName);
  if (texture) {
    m_textureMap[filepathOrName] = texture;
  } else {
    return getDefaultTexture(GENERAL);
  }
  return *texture;
}
const Texture &resources::getConstTexture(const char *filepath)
{
  const auto search = m_textureMap.find(filepath);
  if (search != m_textureMap.end()) {
    return *(search->second);
  }
  Texture *texture = new Texture(filepath);
  if (texture) {
    m_textureMap[filepath] = texture;
    return *std::as_const(texture);
  } else {
    return getDefaultTexture(GENERAL);
  }
}

} // namespace pain
