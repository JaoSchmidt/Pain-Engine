/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


// DefaultTexture.cpp
#pragma once

#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Texture.h"
#include "Misc/TextureSheet.h"
namespace pain
{
struct Renderer2d;
class Renderer3d;

namespace TextureManager
{
enum DefaultTexture {
  General = 0, // a chess like texture
  Error,       // a very ugly and bright texture representing an error
  Blank,       // simple 1x1 pixel texture
  // FONT         // a font atlas that serves as the default texture font
};

void addRendererForDeletingTextures(Renderer2d *renderer);
void addRendererForDeletingTextures(Renderer3d *renderer);

// ---------------------------------------------------------- //
// TextureSheet
// ---------------------------------------------------------- //
TextureSheet &
createWithDimensions(const char *name, const char *texturePath,
                     float spriteWidth, float spriteHeight,
                     std::initializer_list<std::pair<int, int>> coords,
                     bool keepInCPUMemory = false, float padding = 0.5f);
TextureSheet &
createWithDivisions(const char *name, const char *texturePath, unsigned nlinesX,
                    unsigned ncolumnsY,
                    std::initializer_list<std::pair<int, int>> coords,
                    bool keepInCPUMemory = false, float padding = 0.5f);
TextureSheet &getTextureSheet(const char *filepath);
// ---------------------------------------------------------- //
// Texture
// ---------------------------------------------------------- //

// Try to get the texture, if not found try to create the texture
Texture &createTexture(const char *pathOrName, bool clamp = false,
                       bool isPath = true, bool keepOnCPUMemory = false,
                       bool isError = true);

// Try to get the texture, if not found, it will return a default texture
Texture &getTexture(const char *filepath, bool isPath = true,
                    bool isError = true);

// Try to get the texture, if not found, it will return a default texture
const Texture &getConstTexture(const char *pathOrName, bool isPath = true,
                               bool isError = true);

Texture &getDefaultTexture(DefaultTexture defTex, bool isError = true);

Texture &createDumpTexture(const char *name, uint32_t width, uint32_t height,
                           ImageFormat imf = ImageFormat::RGBA8,
                           bool isError = true);
void initiateDefaultTextures();
void clearTextures();
bool deleteTexture(const std::string &key, bool isPath = true);
// std::string getFullPath(const char *relativeOrAbsolute);

} // namespace TextureManager
} // namespace pain
