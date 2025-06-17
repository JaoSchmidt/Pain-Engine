#include "CoreFiles/ResourceManagerSing.h"
#include "CoreFiles/LogWrapper.h"
#include <utility>

namespace pain
{

// NOTE: remember folks, surfaceMap is in the static/global memory but its
// content are in the heap
static std::map<const char *, Texture *> m_textureMap = {};
static std::map<std::pair<uint32_t, uint32_t>, Texture *> m_textureDumpMap = {};
static std::map<const char *, sol::load_result *> m_luaFileMap = {};

sol::load_result &resources::loadLuaFile(sol::state &solstate,
                                         const char *filepath)
{
  auto search = m_luaFileMap.find(filepath);
  if (search != m_luaFileMap.end())
    return *search->second;
  sol::load_result *new_script = solstate.load_file(filepath);
  if (new_script) {
    m_luaFileMap[filepath] = new_script;
  } else {
    PLOG_W("Could not load script on path \"{}\"", filepath);
  }
  return *new_script;
}
Texture &resources::getDumpTexture(uint32_t width, uint32_t height,
                                   ImageFormat imf)
{
  auto search = m_textureDumpMap.find({width, height});
  if (search != m_textureDumpMap.end()) {
    return *(search->second);
  }
  Texture *texture = new Texture(width, height, imf);
  if (texture) {
    m_textureDumpMap[{width, height}] = texture;
  } else {
    return getDefaultTexture(GENERAL);
  }
  return *texture;
}

Texture &resources::getDefaultTexture(resources::DEFAULT_TEXTURE defTex)
{
  PLOG_W("Using a default texture {}", static_cast<int>(defTex));
  switch (defTex) {
  case GENERAL:
    return *m_textureMap.at("resources/textures/defaultTexture.png");
  case ERROR:
    return *m_textureMap.at("resources/textures/defaultTexture.png");
  case FONT:
    return *m_textureMap.at("resources/default/fonts/OpenSans-Regular.ttf");
  }
}

Texture &resources::getTexture(const char *filepath)
{
  auto search = m_textureMap.find(filepath);
  if (search != m_textureMap.end()) {
    return *(search->second);
  }
  Texture *texture = new Texture(filepath);
  if (texture) {
    m_textureMap[filepath] = texture;
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
