#pragma once

#include "CoreRender/Renderer/Renderer2d.h"
#include "CoreRender/Texture.h"
#include "Misc/TextureSheet.h"
namespace pain
{
namespace resources
{
enum DefaultTexture {
  General = 0, // a chess like texture
  Error,       // a very ugly and bright texture representing an error
  Blank,       // simple 1x1 pixel texture
  // FONT         // a font atlas that serves as the default texture font
};
// ---------------------------------------------------------- //
// TextureSheet
// ---------------------------------------------------------- //
TextureSheet &
createTextureSheet(const char *name, const char *texturePath, unsigned nlinesX,
                   unsigned ncolumnsY,
                   std::initializer_list<std::pair<int, int>> coords);
// ---------------------------------------------------------- //
// Texture
// ---------------------------------------------------------- //
Texture &getTexture(const char *filepath, bool gl_clamp_to_edge = false,
                    bool isPath = true);
Texture &getDefaultTexture(DefaultTexture defTex, bool isError = true);
const Texture &getConstTexture(const char *pathOrName, bool isPath = true);
Texture &createDumpTexture(const char *name, uint32_t width, uint32_t height,
                           ImageFormat imf = ImageFormat::RGBA8,
                           bool isError = true);
void initiateDefaultTexture();
void clearTextures();
bool deleteTexture(const std::string &key, Renderer2d &renderer);
void setDefaultPath(const char *path);
std::string getFullPath(const char *relativeOrAbsolute);

} // namespace resources
} // namespace pain
