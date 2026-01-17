/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "CoreRender/Texture.h"
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <vector>

namespace pain
{

/**
 * @class Font
 * @brief GPU-ready font asset backed by an MSDF atlas texture.
 *
 * The Font class loads a font file using **FreeType + msdf-atlas-gen**,
 * generates a multi-channel signed distance field (MSDF) atlas,
 * and uploads the result into a GPU Texture for fast text rendering.
 *
 * ---
 * ### ✨ Responsibilities
 * - Load font files from disk.
 * - Generate glyph geometry and layout.
 * - Build MSDF atlas textures.
 * - Expose glyph metrics and atlas texture to the renderer.
 *
 * ---
 * ### ⚠️ Ownership
 * - Instances are heap-allocated via `create()`.
 * - Callers own the returned pointer.
 * - `getDefault()` returns a static fallback instance (do not delete).
 *
 * ---
 * ### 🚀 Performance Notes
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

  /** Returns the generated glyph geometry array. */
  const inline std::vector<msdf_atlas::GlyphGeometry> &getGlyphGemoetry() const
  {
    return m_glyphs;
  }

  /**
   * @brief Returns the engine default font.
   *
   * The default font is lazily initialized and persists for the lifetime
   * of the application.
   *
   * @return Pointer to the default Font instance.
   */
  static Font *getDefault();

private:
  // ---------------------------------------------------------------------------
  // Data
  // ---------------------------------------------------------------------------

  /** Font metrics and glyph layout data. */
  msdf_atlas::FontGeometry m_fontGeometry;
  /** Geometry information for each glyph in the atlas. */
  std::vector<msdf_atlas::GlyphGeometry> m_glyphs;
  /** GPU texture storing the MSDF atlas. */
  Texture m_atlasTexture;

  /** Global fallback font instance. */
  static Font *m_defaultFont;

  // ---------------------------------------------------------------------------
  // Constants
  // ---------------------------------------------------------------------------

  /** Texture cache key used internally. */
  const char *textureKey = "fontAltas";
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
  msdf_atlas::Charset getLatinCharset();

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
                     const int width, const int height,
                     const char *fontFilename);

  /** Constructs a font and immediately generates its atlas. */
  Font(const char *fontFilename, double emSize);
};

} // namespace pain
