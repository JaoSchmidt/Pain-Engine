#include "Assets/DefaultTexture.h"
#include "CoreFiles/LogWrapper.h"

namespace pain
{

bool m_enableDefaultTextureWarning = false;
static std::map<const char *, Texture *> m_textureMap = {};
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
Texture &resources::getDefaultTexture(resources::DEFAULT_TEXTURE defTex)
{
  P_ASSERT(m_textureMap.contains("BLANK"),
           "Some default values are missing, did you remember to call "
           "initateDefaultValues?")
  if (m_enableDefaultTextureWarning)
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

void resources::initiateDefaultTexture()
{
  Texture &whiteTexture = resources::createDumpTexture("BLANK", 1, 1);
  const uint32_t whiteTextureData = 0xffffffff;
  whiteTexture.setData(&whiteTextureData, sizeof(uint32_t));
  m_enableDefaultTextureWarning = true;
}
} // namespace pain
