#pragma once

#include "CoreRender/Texture.h"

namespace pain
{
namespace resources
{
enum DEFAULT_TEXTURE {
  GENERAL = 0, // a chess like texture
  ERROR,       // a very ugly and bright texture representing an error
  BLANK,       // simple 1x1 pixel texture
  // FONT         // a font atlas that serves as the default texture font
};
Texture &getTexture(const char *filepath, bool gl_clamp_to_edge = false);
Texture &getDefaultTexture(DEFAULT_TEXTURE defTex);
const Texture &getConstTexture(const char *filepath);
Texture &createDumpTexture(const char *name, uint32_t width, uint32_t height,
                           ImageFormat imf = ImageFormat::RGBA8);
void initiateDefaultTexture();
void clearTextures();

} // namespace resources
} // namespace pain
