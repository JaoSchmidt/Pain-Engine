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

enum DEFAULT_TEXTURE {
  GENERAL = 0, // a chess like texture
  ERROR,       // a very ugly and bright texture representing an error
  BLANK,       // simple 1x1 pixel texture
  // FONT         // a font atlas that serves as the default texture font
};

const char *getDefaultLuaFile();
const std::string &getLuaScriptSource(const char *filepath);
Texture &createDumpTexture(const char *name, uint32_t width, uint32_t height,
                           ImageFormat imf = ImageFormat::RGBA8);
void initiateDefaultValues(sol::state &solstate);
Texture &getTexture(const char *filepath);
const Texture &getConstTexture(const char *filepath);
Texture &getDefaultTexture(DEFAULT_TEXTURE defTex);
const Texture &getDefaultConstTexture(DEFAULT_TEXTURE defTex);
} // namespace resources

} // namespace pain
