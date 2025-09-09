#include "Assets/DefaultTexture.h"
#include "CoreFiles/LogWrapper.h"
#include <optional>

namespace pain
{

bool m_enableDefaultTextureWarning = false;
static std::map<std::string, Texture> m_textureMap = {};
Texture &resources::createDumpTexture(const char *name, uint32_t width,
                                      uint32_t height, ImageFormat imf)
{
  auto search = m_textureMap.find(name);
  if (search != m_textureMap.end()) {
    PLOG_W("You are trying to create texture {}, but it already was created",
           name);
    return search->second;
  }
  auto textOpt = Texture::createTexture(name, width, height, imf);
  if (textOpt) {
    auto [it, inserted] = m_textureMap.emplace(name, std::move(*textOpt));
    return it->second;
  } else {
    return getDefaultTexture(GENERAL);
  }
}
Texture &resources::getDefaultTexture(resources::DEFAULT_TEXTURE defTex)
{
  P_ASSERT(m_textureMap.contains("BLANK"),
           "Some default values are missing, did you remember to call "
           "initateDefaultValues?")
  if (m_enableDefaultTextureWarning)
    PLOG_W("Using a default texture {}", static_cast<int>(defTex));
  switch (defTex) {
  case GENERAL:
    return m_textureMap.at("resources/default/textures/defaultTexture.png");
  case ERROR:
    return m_textureMap.at("resources/default/textures/defaultTexture.png");
  case BLANK:
    return m_textureMap.at("BLANK");
  }
}
const Texture &resources::getConstTexture(const char *filepath)
{
  const auto search = m_textureMap.find(filepath);
  if (search != m_textureMap.end()) {
    return std::as_const(search->second);
  }
  auto textOpt = Texture::createTexture(filepath);
  if (textOpt) {
    auto [it, inserted] = m_textureMap.emplace(filepath, std::move(*textOpt));
    return std::as_const(it->second);
  } else {
    return std::as_const(getDefaultTexture(GENERAL));
  }
}
Texture &resources::getTexture(const char *pathOrName, bool gl_clamp_to_edge)
{
  const auto search = m_textureMap.find(pathOrName);
  if (search != m_textureMap.end()) {
    return search->second;
  }
  auto textOpt = Texture::createTexture(pathOrName, gl_clamp_to_edge);
  if (textOpt) {
    auto [it, inserted] = m_textureMap.emplace(pathOrName, std::move(*textOpt));
    return it->second;
  } else {
    return getDefaultTexture(GENERAL);
  }
}

void resources::initiateDefaultTexture()
{
  Texture &whiteTexture = resources::createDumpTexture("BLANK", 1, 1);
  const uint32_t whiteTextureData = 0xffffffff;
  whiteTexture.setData(&whiteTextureData, sizeof(uint32_t));
  m_enableDefaultTextureWarning = true;
}

void resources::clearTextures() { m_textureMap.clear(); }
} // namespace pain
