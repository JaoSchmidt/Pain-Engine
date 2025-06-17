#pragma once
#include "CoreRender/Texture.h"
#include "pch.h"
#include <sol/state.hpp>

namespace pain
{

// The resources manager. A simple API to concentrate all interactions with
// heavy resoruces. While also avoiding potentials fatal errors
namespace resources
{
SDL_Surface *getSurface(const char *filepath);
sol::load_result &loadLuaFile(sol::state &solstate, const char *filepath);

enum DEFAULT_TEXTURE {
  GENERAL = 0, // a blank simple texture with nothing inside
  ERROR,       // a very ugly and bright texture representing an error
  FONT         // a font atlas that serves as the default texture font
};
Texture &getTexture(const char *filepath);
const Texture &getConstTexture(const char *filepath);
Texture &getDefaultTexture(DEFAULT_TEXTURE defTex);
const Texture &getDefaultConstTexture(DEFAULT_TEXTURE defTex);
Texture &getDumpTexture(uint32_t width, uint32_t height, ImageFormat imf);
const Texture &getDumpConstTexture(uint32_t width, uint32_t height,
                                   ImageFormat imf);
} // namespace resources

} // namespace pain
