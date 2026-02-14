/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// ManagerTexture.cpp
#include "Assets/ManagerTexture.h"
#include "Assets/ManagerFile.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/RenderContext.h"
#include "Misc/TextureSheet.h"

#include <exception>
#include <filesystem>
#include <optional>
#define ERROR_TEXTURE_PATH "resources/default/textures/errorTexture.png"
#define GENERAL_TEXTURE_PATH                                                   \
  "resources/default/textures/defaultTexture_colors.png"

#define GENERAL_KEY "GENERAL"
#define BLANK_KEY "BLANK"
#define ERROR_KEY "ERROR"

namespace pain
{
namespace
{
std::map<std::string, Texture> s_textureMap = {};
std::map<std::string, TextureSheet> s_textureSheetMap = {};
Renderer2d *s_renderer2d;
Renderer3d *s_renderer3d;
} // namespace

void TextureManager::addRendererForDeletingTextures(Renderers &renderers)
{
  s_renderer2d = &renderers.renderer2d;
  s_renderer3d = &renderers.renderer3d;
}

// ---------------------------------------------------------- //
// Renderers
// ---------------------------------------------------------- //

std::string getFullPath(const char *relativeOrAbsolute)
{
  namespace fs = std::filesystem;
  fs::path p(relativeOrAbsolute);

  // If already absolute, just normalize it
  if (p.is_absolute()) {
    return fs::weakly_canonical(p).string();
  }

  fs::path base(FileManager::getDefaultPath());
  fs::path full = base / p;
  return fs::weakly_canonical(full).string();
}

void createDefaultTexture(TextureManager::DefaultTexture d, const char *path)
{
  auto texOpt = Texture::createTexture(path, false, false);
  std::string key = "";
  switch (d) {
  case TextureManager::DefaultTexture::General:
    key = GENERAL_KEY;
    break;
  case TextureManager::DefaultTexture::Error:
    key = ERROR_KEY;
    break;
  case TextureManager::DefaultTexture::Blank:
    key = BLANK_KEY;
    break;
  }
  auto [_, inserted] = s_textureMap.emplace(key, std::move(*texOpt));
  if (!inserted) {
    PLOG_E("Could not create default {} texture", key);
    std::terminate();
  }
}

void TextureManager::initiateDefaultTextures()
{
  // White texture
  Texture &whiteTexture = TextureManager::createDumpTexture(
      BLANK_KEY, 1, 1, ImageFormat::RGBA8, false);
  const uint32_t whiteTextureData = 0xffffffff;
  whiteTexture.setData(&whiteTextureData, sizeof(uint32_t));

  // Default textures
  createDefaultTexture(TextureManager::DefaultTexture::General,
                       GENERAL_TEXTURE_PATH);
  createDefaultTexture(TextureManager::DefaultTexture::Error,
                       ERROR_TEXTURE_PATH);
  createWithDivisions(GENERAL_KEY, GENERAL_TEXTURE_PATH, 1, 1, {{0, 0}});
}

// ---------------------------------------------------------- //
// TextureSheet
// ---------------------------------------------------------- //
TextureSheet &TextureManager::createWithDimensions(
    const char *name, const char *texturePath, float spriteWidth,
    float spriteHeight, std::initializer_list<std::pair<int, int>> coords,
    bool keepOnCPUMemory, float padding)
{
  auto search = s_textureSheetMap.find(name);
  if (search != s_textureSheetMap.end()) {
    PLOG_W("TextureSheet {} already exists", name);
    return search->second;
  }

  Texture &tex =
      TextureManager::createTexture(texturePath, false, true, keepOnCPUMemory);
  TextureSheet sheet = TextureSheet::createWithDimensions(
      tex, spriteWidth, spriteHeight, coords, padding);

  auto [it, inserted] = s_textureSheetMap.emplace(name, std::move(sheet));
  if (!inserted)
    PLOG_E("Could not insert sprite sheet {}", name);
  return it->second;
}
TextureSheet &TextureManager::createWithDivisions(
    const char *name, const char *texturePath, unsigned nlinesX,
    unsigned ncolumnsY, std::initializer_list<std::pair<int, int>> coords,
    bool keepOnCPUMemory, float padding)
{
  auto search = s_textureSheetMap.find(name);
  if (search != s_textureSheetMap.end()) {
    PLOG_W("TextureSheet {} already exists", name);
    return search->second;
  }

  Texture &tex =
      TextureManager::createTexture(texturePath, false, true, keepOnCPUMemory);
  TextureSheet sheet = TextureSheet::createWithDivisions(
      tex, nlinesX, ncolumnsY, coords, padding);

  auto [it, inserted] = s_textureSheetMap.emplace(name, std::move(sheet));
  return it->second;
}
TextureSheet &getDefaultSheet(TextureManager::DefaultTexture defTex,
                              bool isError = true)
{
  P_ASSERT(s_textureSheetMap.contains(GENERAL_KEY),
           "Some default values are missing, did you remember to call "
           "initiateDefaultTextures?")

  switch (defTex) {
  case TextureManager::DefaultTexture::General:
  case TextureManager::DefaultTexture::Blank:
  case TextureManager::DefaultTexture::Error: {
    if (isError)
      PLOG_E("Using a default ERROR texture sheet");
    return s_textureSheetMap.at(ERROR_KEY);
  }
  }
}

TextureSheet &TextureManager::getTextureSheet(const char *name)
{
  auto search = s_textureSheetMap.find(name);
  if (search != s_textureSheetMap.end()) {
    return search->second;
  }
  return getDefaultSheet(DefaultTexture::General);
}

// ---------------------------------------------------------- //
// Texture
// ---------------------------------------------------------- //

Texture &TextureManager::createDumpTexture(const char *name, uint32_t width,
                                           uint32_t height, ImageFormat imf,
                                           bool isError)
{
  auto search = s_textureMap.find(name);
  if (search != s_textureMap.end()) {
    PLOG_W("You are trying to create texture {}, but it already was created",
           name);
    return search->second;
  }
  auto textOpt = Texture::createTexture(name, width, height, imf);
  if (textOpt) {
    auto [it, inserted] = s_textureMap.emplace(name, std::move(*textOpt));
    return it->second;
  } else {
    PLOG_W("Using a default Texture instead of {}", name);
    return getDefaultTexture(DefaultTexture::General, isError);
  }
}
Texture &TextureManager::getDefaultTexture(DefaultTexture defTex, bool isError)
{
  P_ASSERT(s_textureMap.contains("BLANK"),
           "Some default values are missing, did you remember to call "
           "initiateDefaultTextures?")

  switch (defTex) {
  case DefaultTexture::General: {
    if (isError)
      PLOG_W("Using a default GENERAL texture");
    return s_textureMap.at(GENERAL_KEY);
  }
  case DefaultTexture::Error: {
    auto t = getFullPath(ERROR_TEXTURE_PATH);
    if (isError)
      PLOG_E("Using a default ERROR texture");
    return s_textureMap.at(ERROR_KEY);
  }
  case DefaultTexture::Blank:
    if (isError)
      PLOG_W("Using a default blank texture");
    return s_textureMap.at(BLANK_KEY);
  }
}

Texture &TextureManager::createTexture(const char *pathOrName, bool clamp,
                                       bool isPath, bool keepOnCPUMemory,
                                       bool isError)
{
  std::string key = "";
  if (isPath)
    key = getFullPath(pathOrName);
  else
    key = pathOrName;
  const auto search = s_textureMap.find(key);
  if (search != s_textureMap.end()) {
    return search->second;
  }
  auto textOpt =
      Texture::createTexture(key.c_str(), clamp, keepOnCPUMemory, isError);
  if (textOpt) {
    auto [it, inserted] = s_textureMap.emplace(key, std::move(*textOpt));
    if (inserted)
      return it->second;
  }
  if (isError)
    PLOG_E("Error creating texture, using a default Texture instead of {}",
           key);
  return getDefaultTexture(DefaultTexture::General, isError);
}

const Texture &TextureManager::getConstTexture(const char *pathOrName,
                                               bool isPath, bool isError)
{
  std::string key = "";
  if (isPath)
    key = getFullPath(pathOrName);
  else
    key = pathOrName;
  const auto search = s_textureMap.find(key);
  if (search != s_textureMap.end())
    return std::as_const(search->second);
  if (isError)
    PLOG_E("Error loading texture, using a default Texture instead of {}", key);
  return std::as_const(getDefaultTexture(DefaultTexture::General));
}
Texture &TextureManager::getTexture(const char *pathOrName, bool isPath,
                                    bool isError)
{
  std::string key = "";
  if (isPath)
    key = getFullPath(pathOrName);
  else
    key = pathOrName;
  const auto search = s_textureMap.find(key);
  if (search != s_textureMap.end()) {
    return search->second;
  }
  if (isError)
    PLOG_E("Error loading texture, using a default Texture instead of {}", key);
  return getDefaultTexture(DefaultTexture::General, isError);
}

bool TextureManager::deleteTexture(const std::string &pathOrName, bool isPath)
{
  std::string key;
  if (isPath)
    key = getFullPath(pathOrName.c_str());
  else
    key = pathOrName;

  auto search = s_textureMap.find(key);

  if (search == s_textureMap.end()) {
    PLOG_W("Tried to delete texture {}, but it does not exist in the map", key);
    return false;
  }
  Texture &tex = search->second;
  if (s_renderer2d) {
    s_renderer2d->removeTexture(tex);
  } else {
    PLOG_W("Texture was not removed inside renderer");
  }
  s_textureMap.erase(search);
  PLOG_I("Deleted texture {}", key);
  return true;
}

void TextureManager::clearTextures() { s_textureMap.clear(); }
} // namespace pain
