/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// ManagerTexture.h
#pragma once

#include "CoreRender/Renderer/Misc.h"
#include "CoreRender/Texture.h"
#include "Misc/TextureSheet.h"

namespace pain
{
struct Renderers;

/**
 * @namespace TextureManager
 * @brief Centralized manager for loading, creating, caching and deleting
 * textures and texture sheets.
 *
 * This namespace owns all textures and texture sheets created through it.
 * Resources are cached internally and reused when requested multiple times.
 *
 * Default textures must be initialized by calling initiateDefaultTextures()
 * before normal usage.
 */
namespace TextureManager
{

/**
 * @enum DefaultTexture
 * @brief Identifiers for built-in default textures.
 */
enum DefaultTexture {
  /** Chess-like placeholder texture. */
  General = 0,

  /** Bright error texture used when loading fails. */
  Error,

  /** Simple 1x1 white texture. */
  Blank,
};

/**
 * @brief Registers the 2D and 3D renderers for texture deletion callbacks.
 *
 * When a texture is deleted from the manager, the renderer will be notified
 * so that GPU-side references can be released.
 *
 * @param renderer Pointer to the Renderer2d instance.
 */
void addRendererForDeletingTextures(Renderers &renderers);

// ---------------------------------------------------------- //
// TextureSheet
// ---------------------------------------------------------- //

/**
 * @brief Creates or retrieves a TextureSheet using explicit sprite dimensions.
 *
 * If a texture sheet with the same name already exists, the existing one is
 * returned instead of creating a new one.
 *
 * @param name              Unique name used as the cache key.
 * @param texturePath       Path to the source texture.
 * @param spriteWidth       Width of a single sprite in pixels.
 * @param spriteHeight      Height of a single sprite in pixels.
 * @param coords            List of sprite coordinates to extract.
 * @param keepInCPUMemory   Whether the texture data should remain in CPU
 * memory.
 * @param padding           Optional UV padding to avoid texture bleeding.
 *
 * @return Reference to the created or cached TextureSheet.
 */
TextureSheet &
createWithDimensions(const char *name, const char *texturePath,
                     float spriteWidth, float spriteHeight,
                     std::initializer_list<std::pair<int, int>> coords,
                     bool keepInCPUMemory = false, float padding = 0.5f);

/**
 * @brief Creates or retrieves a TextureSheet using grid divisions.
 *
 * If a texture sheet with the same name already exists, the existing one is
 * returned instead of creating a new one.
 *
 * @param name              Unique name used as the cache key.
 * @param texturePath       Path to the source texture.
 * @param nlinesX           Number of horizontal divisions.
 * @param ncolumnsY         Number of vertical divisions.
 * @param coords            List of sprite coordinates to extract.
 * @param keepInCPUMemory   Whether the texture data should remain in CPU
 * memory.
 * @param padding           Optional UV padding to avoid texture bleeding.
 *
 * @return Reference to the created or cached TextureSheet.
 */
TextureSheet &
createWithDivisions(const char *name, const char *texturePath, unsigned nlinesX,
                    unsigned ncolumnsY,
                    std::initializer_list<std::pair<int, int>> coords,
                    bool keepInCPUMemory = false, float padding = 0.5f);

/**
 * @brief Retrieves a texture sheet by name.
 *
 * If the texture sheet does not exist, a default texture sheet is returned.
 *
 * @param filepath Name of the texture sheet.
 * @return Reference to the texture sheet.
 */
TextureSheet &getTextureSheet(const char *filepath);

// ---------------------------------------------------------- //
// Texture
// ---------------------------------------------------------- //

/**
 * @brief Creates or retrieves a texture.
 *
 * If the texture already exists in the cache, it is returned directly.
 * Otherwise, the texture is created and stored internally.
 *
 * @param pathOrName        File path or unique name.
 * @param clamp             Whether texture sampling should be clamped.
 * @param isPath            If true, pathOrName is treated as a filesystem path.
 * @param keepOnCPUMemory   Whether texture data should remain in CPU memory.
 * @param isError           Whether errors should be logged and default fallback
 * used.
 *
 * @return Reference to the created or cached Texture.
 */
Texture &createTexture(const char *pathOrName, bool clamp = false,
                       bool isPath = true, bool keepOnCPUMemory = false,
                       bool isError = true);

/**
 * @brief Retrieves a texture if it exists, otherwise returns a default texture.
 *
 * @param filepath  File path or unique name.
 * @param isPath    If true, filepath is treated as a filesystem path.
 * @param isError   Whether errors should be logged.
 *
 * @return Reference to the texture or a default texture.
 */
Texture &getTexture(const char *filepath, bool isPath = true,
                    bool isError = true);

/**
 * @brief Retrieves a const reference to a texture.
 *
 * If the texture does not exist, a default texture is returned.
 *
 * @param pathOrName  File path or unique name.
 * @param isPath      If true, pathOrName is treated as a filesystem path.
 * @param isError     Whether errors should be logged.
 *
 * @return Const reference to the texture.
 */
const Texture &getConstTexture(const char *pathOrName, bool isPath = true,
                               bool isError = true);

/**
 * @brief Retrieves one of the built-in default textures.
 *
 * @param defTex   Which default texture to retrieve.
 * @param isError  Whether usage should generate warning logs.
 *
 * @return Reference to the requested default texture.
 */
Texture &getDefaultTexture(DefaultTexture defTex, bool isError = true);

/**
 * @brief Creates a blank texture with explicit dimensions.
 *
 * If a texture with the same name already exists, the existing one is returned.
 *
 * @param name     Unique name used as the cache key.
 * @param width    Texture width in pixels.
 * @param height   Texture height in pixels.
 * @param imf       Image format.
 * @param isError   Whether errors should be logged.
 *
 * @return Reference to the created or cached Texture.
 */
Texture &createDumpTexture(const char *name, uint32_t width, uint32_t height,
                           ImageFormat imf = ImageFormat::RGBA8,
                           bool isError = true);

/**
 * @brief Initializes all built-in default textures.
 *
 * This function must be called before requesting default textures.
 */
void initiateDefaultTextures();

/**
 * @brief Deletes all cached textures.
 *
 * Renderer references are not notified when using this function.
 */
void clearTextures();

/**
 * @brief Deletes a texture from the manager.
 *
 * The registered renderers will be notified so GPU resources can be released.
 *
 * @param key      File path or name of the texture.
 * @param isPath   If true, key is treated as a filesystem path.
 *
 * @return True if the texture was deleted successfully, false otherwise.
 */
bool deleteTexture(const std::string &key, bool isPath = true);

} // namespace TextureManager
} // namespace pain
