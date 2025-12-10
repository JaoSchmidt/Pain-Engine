#include "Assets/DefaultTexture.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "Misc/TextureSheet.h"

#include <filesystem>
#include <optional>

namespace pain
{
namespace
{
// Renderer2d *renderer2d = nullptr;
// Renderer3d *renderer3d;
std::map<std::string, Texture> m_textureMap = {};
std::string m_resourcesPath; // we can replace this with IniConfig class
                             // instance but not really in a hurry tbh
std::map<std::string, TextureSheet> m_textureSheetMap = {};
} // namespace

// ---------------------------------------------------------- //
// TextureSheet
// ---------------------------------------------------------- //
TextureSheet &
resources::createTextureSheet(const char *name, const char *texturePath,
                              unsigned nlinesX, unsigned ncolumnsY,
                              std::initializer_list<std::pair<int, int>> coords)
{
  auto search = m_textureSheetMap.find(name);
  if (search != m_textureSheetMap.end()) {
    PLOG_W("TextureSheet {} already exists", name);
    return search->second;
  }

  Texture &tex = getTexture(texturePath);
  TextureSheet sheet =
      TextureSheet::createTextureSheet(tex, nlinesX, ncolumnsY, coords);

  auto [it, inserted] = m_textureSheetMap.emplace(name, std::move(sheet));
  return it->second;
}

TextureSheet &getTextureSheet(const char *name)
{
  auto search = m_textureSheetMap.find(name);
  P_ASSERT(search != m_textureSheetMap.end(), "TextureSheet {} not found",
           name);
  return search->second;
}

void clearTextureSheets() { m_textureSheetMap.clear(); }
// ---------------------------------------------------------- //
// Texture
// ---------------------------------------------------------- //

void resources::setDefaultPath(const char *path)
{
  namespace fs = std::filesystem;
  fs::path p(path);
  // normalize path (remove redundant ./ and ensure trailing slash)
  p = fs::weakly_canonical(p);
  if (p.filename() == "resources") {
    p = p.parent_path();
  }
  m_resourcesPath = p.string();
}

std::string resources::getFullPath(const char *relativeOrAbsolute)
{
  namespace fs = std::filesystem;
  fs::path p(relativeOrAbsolute);

  // If already absolute, just normalize it
  if (p.is_absolute()) {
    return fs::weakly_canonical(p).string();
  }

  fs::path base(m_resourcesPath);
  fs::path full = base / p;
  return fs::weakly_canonical(full).string();
}
Texture &resources::createDumpTexture(const char *name, uint32_t width,
                                      uint32_t height, ImageFormat imf,
                                      bool isError)
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
    // prevents m_textureSlots become invalid by allocating at least once;
    // if (renderer2d != nullptr) {
    //   renderer2d->allocateTextures(*textOpt);
    // } else if (renderer3d != nullptr) {
    //   PLOG_E("allocate texture for renderer 3d is WIP");
    //   exit(1);
    // } else {
    //   PLOG_E(
    //       "Did you forget to allocate the renderer inside the asset
    //       manager?");
    //   exit(1);
    // }
    return it->second;
  } else {
    PLOG_W("Using a default Texture instead of {}", name);
    return getDefaultTexture(GENERAL, isError);
  }
}
Texture &resources::getDefaultTexture(resources::DEFAULT_TEXTURE defTex,
                                      bool isError)
{
  P_ASSERT(m_textureMap.contains("BLANK"),
           "Some default values are missing, did you remember to call "
           "initiateDefaultTexture?")

  switch (defTex) {
  case GENERAL:
  case ERROR: {
    auto t = getFullPath("resources/default/textures/defaultTexture.png");
    if (isError)
      PLOG_E("Using a default ERROR texture {}", t);
    return m_textureMap.at(t);
  }
  case BLANK:
    if (isError)
      PLOG_W("Using a default blank texture");
    return m_textureMap.at("BLANK");
  }
  // Should never happen
  return m_textureMap.at("BLANK");
}
const Texture &resources::getConstTexture(const char *pathOrName, bool isPath)
{
  std::string key = "";
  if (isPath)
    key = getFullPath(pathOrName);
  else
    key = pathOrName;
  const auto search = m_textureMap.find(key);
  if (search != m_textureMap.end()) {
    return std::as_const(search->second);
  }
  auto textOpt = Texture::createTexture(key.c_str());
  if (textOpt) {
    auto [it, inserted] = m_textureMap.emplace(key, std::move(*textOpt));
    return std::as_const(it->second);
  } else {
    PLOG_W("Using a default Texture instead of {}", key);
    return std::as_const(getDefaultTexture(GENERAL));
  }
}
Texture &resources::getTexture(const char *pathOrName, bool gl_clamp_to_edge,
                               bool isPath)
{
  std::string key = "";
  if (isPath)
    key = getFullPath(pathOrName);
  else
    key = pathOrName;
  const auto search = m_textureMap.find(key);
  if (search != m_textureMap.end()) {
    return search->second;
  }
  auto textOpt = Texture::createTexture(key.c_str(), gl_clamp_to_edge);
  if (textOpt) {
    auto [it, inserted] = m_textureMap.emplace(key, std::move(*textOpt));
    return it->second;
  } else {
    PLOG_W("Using a default Texture instead of {}", key);
    return getDefaultTexture(GENERAL);
  }
}

void resources::initiateDefaultTexture()
{
  getTexture("resources/default/textures/defaultTexture.png");
  Texture &whiteTexture =
      resources::createDumpTexture("BLANK", 1, 1, ImageFormat::RGBA8, false);
  const uint32_t whiteTextureData = 0xffffffff;
  whiteTexture.setData(&whiteTextureData, sizeof(uint32_t));
}

bool resources::deleteTexture(const std::string &key, Renderer2d &renderer)
{
  auto search = m_textureMap.find(key);
  if (search == m_textureMap.end()) {
    PLOG_W("Tried to delete texture {}, but it does not exist in the map", key);
    return false;
  }
  Texture &tex = search->second;
  renderer.removeTexture(tex);
  m_textureMap.erase(search);
  PLOG_I("Deleted texture {}", key);
  return true;
}

void resources::clearTextures() { m_textureMap.clear(); }
} // namespace pain
