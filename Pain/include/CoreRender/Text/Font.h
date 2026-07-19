/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Core.h"
#include "CoreRender/Buffers/Texture.h"
#include <memory>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <vector>

namespace pain
{

/**
 * @class Font
 * @brief GPU-ready font asset to render text
 *
 * The Font class loads a font file using **FreeType + msdf-atlas-gen**,
 * generates a multi-channel signed distance field (MSDF) atlas,
 * and uploads the result into a GPU Texture for fast text rendering. The MSDF
 * allow to create fonts with any size possible, using only one atlas, as
 * opposed to a bunch of different atlas, each for a font sizes
 *
 * ---
 * ### Responsibilities
 * - Load font files from disk (*.ttf or *.otf).
 * - Internally fenerate glyph geometry, layout, and build MSDF atlas textures.
 * - Require to Render text in the renderers (Renderer2d.h and Renderer3d.h)
 *
 * ---
 * ### Ownership
 * - You deal with it, all returns are rvalue
 * - OR you use ManagerFont.h (recommended)
 *
 * ---
 * ### Performance Notes
 * - Atlas generation is expensive and should be done during load time,
 *   not during gameplay or hot paths.
 * - Texture data is uploaded once and reused for rendering.
 */
class Font
{
public:
  /**
   * @brief Creates a font using a default em size.
   *
   * If loading fails, a fallback default font is returned.
   *
   * @param fontFilename Path to the font file.
   * @return Pointer to a Font instance (never null).
   */
  static Font *create(const char *fontFilename);

  /**
   * @brief Creates a font using a custom em size.
   *
   * If loading fails, a fallback default font is returned.
   *
   * @param fontFilename Path to the font file.
   * @param emSize        Target glyph scale in pixels.
   * @return Pointer to a Font instance (never null).
   */
  static Font *create(const char *fontFilename, double emSize);

  /** Returns the GPU atlas texture (read-only). */
  const inline Texture &getAtlasTexture() const { return m_atlasTexture; }

  /** Returns the GPU atlas texture (mutable). */
  inline Texture &getAtlasTexture() { return m_atlasTexture; }

  /** Returns the font geometry describing metrics and layout. */
  const inline msdf_atlas::FontGeometry &getFontGeometry() const
  {
    return m_fontGeometry;
  }

  Font(Font &&) noexcept = default;
  Font &operator=(Font &&) noexcept = default;
  Font &operator=(Font &) = delete;
  Font(Font &) = delete;

private:
  // ---------------------------------------------------------------------------
  // Data
  // ---------------------------------------------------------------------------

  /* Geometry information for each glyph in the atlas.
   * WARN: DO NOT USE, for dispose only inside the custom destructor */
  std::unique_ptr<std::vector<msdf_atlas::GlyphGeometry>> m_glyphs;
  /** Font metrics and glyph layout data. */
  msdf_atlas::FontGeometry m_fontGeometry;
  /** GPU texture storing the MSDF atlas. */
  Texture m_atlasTexture;

  // ---------------------------------------------------------------------------
  // Constants
  // ---------------------------------------------------------------------------

  /** Distance field pixel range used during atlas generation. */
  static constexpr double pixelRange = 2.0;
  /** Glyph scaling factor. */
  static constexpr double glyphScale = 32.0;
  /** Miter limit for edge coloring. */
  static constexpr double miterLimit = 1.0;
  /** Maximum corner angle for edge coloring. */
  static constexpr double maxCornerAngle = 3.0;

  // ---------------------------------------------------------------------------
  // Internal Methods
  // ---------------------------------------------------------------------------

  /**
   * @brief Generates an MSDF atlas texture from a font file.
   *
   * Loads the font via FreeType, generates glyph geometry,
   * packs glyphs into an atlas, and uploads the bitmap to a GPU texture.
   *
   * @param fontFilename Path to the font file.
   * @param emSize        Target glyph scale.
   * @return Generated atlas texture.
   */
  Texture generateAtlas(const char *fontFilename, double emSize);

  /** Builds the default Latin character set used for glyph loading. */
  static msdf_atlas::Charset getLatinCharset();

  /**
   * @brief Converts generated MSDF atlas data into a GPU texture.
   *
   * This is a templated helper used to support different generator
   * configurations provided by msdf-atlas-gen.
   */
  template <typename T, typename S, int N,
            msdf_atlas::GeneratorFunction<S, N> GenFunc>
  Texture
  createAtlasTexture(const std::vector<msdf_atlas::GlyphGeometry> &glyphs,
                     int width, int height, const char *fontFilename);

  /** Constructs a font and immediately generates its atlas. */
  Font(const char *fontFilename, double emSize);
};

} // namespace pain
