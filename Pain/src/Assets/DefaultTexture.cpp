#include "Assets/DefaultTexture.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Renderer/Renderer3d.h"
#include "Misc/TextureSheet.h"

#include <filesystem>
#include <optional>
#define DEFAULT_TEXTURE_PATH "resources/default/textures/defaultTexture.png"
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
std::string getFullPath(const char *relativeOrAbsolute)
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

void resources::initiateDefaultTextures()
{
  // White texture
  Texture &whiteTexture =
      resources::createDumpTexture("BLANK", 1, 1, ImageFormat::RGBA8, false);
  const uint32_t whiteTextureData = 0xffffffff;
  whiteTexture.setData(&whiteTextureData, sizeof(uint32_t));

  // Error textures
  getTexture(DEFAULT_TEXTURE_PATH);
  createTextureSheet("GENERAL", DEFAULT_TEXTURE_PATH, 1, 1, {{0, 0}});
}

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
TextureSheet &getDefaultSheet(resources::DefaultTexture defTex,
                              bool isError = true)
{
  P_ASSERT(m_textureSheetMap.contains("GENERAL"),
           "Some default values are missing, did you remember to call "
           "initiateDefaultTextures?")

  switch (defTex) {
  case resources::DefaultTexture::General:
  case resources::DefaultTexture::Blank:
  case resources::DefaultTexture::Error: {
    if (isError)
      PLOG_E("Using a default ERROR texture sheet");
    return m_textureSheetMap.at("GENERAL");
  }
  }
}

TextureSheet &resources::getTextureSheet(const char *name)
{
  auto search = m_textureSheetMap.find(name);
  if (search != m_textureSheetMap.end()) {
    return search->second;
  }
  return getDefaultSheet(DefaultTexture::General);
}

// ---------------------------------------------------------- //
// Texture
// ---------------------------------------------------------- //

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
    return it->second;
  } else {
    PLOG_W("Using a default Texture instead of {}", name);
    return getDefaultTexture(DefaultTexture::General, isError);
  }
}
Texture &resources::getDefaultTexture(resources::DefaultTexture defTex,
                                      bool isError)
{
  P_ASSERT(m_textureMap.contains("BLANK"),
           "Some default values are missing, did you remember to call "
           "initiateDefaultTextures?")

  switch (defTex) {
  case DefaultTexture::General:
  case DefaultTexture::Error: {
    auto t = getFullPath(DEFAULT_TEXTURE_PATH);
    if (isError)
      PLOG_E("Using a default ERROR texture {}", t);
    return m_textureMap.at(t);
  }
  case DefaultTexture::Blank:
    if (isError)
      PLOG_W("Using a default blank texture");
    return m_textureMap.at("BLANK");
  }
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
    return std::as_const(getDefaultTexture(DefaultTexture::General));
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
    return getDefaultTexture(DefaultTexture::General);
  }
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
