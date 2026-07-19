// /*
//  * This Source Code Form is subject to the terms of the Mozilla Public
//  * License, v. 2.0. If a copy of the MPL was not distributed with this
//  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  */
//
// #pragma once
//
// #include "CoreRender/Buffers/Texture.h"
// #include <msdf-atlas-gen/msdf-atlas-gen.h>
// #include <vector>
//
// namespace pain
// {
//
// /**
//  * @class Font
//  * @brief GPU-ready font asset backed by an MSDF atlas texture.
//  *
//  * The Font class loads a font file using **FreeType + msdf-atlas-gen**,
//  * generates a multi-channel signed distance field (MSDF) atlas,
//  * and uploads the result into a GPU Texture for fast text rendering.
//  */
// class Font
// {
// public:
//   Font(const Font &) = delete;
//   Font &operator=(const Font &) = delete;
//   Font(Font &&) noexcept = default;
//   Font &operator=(Font &&) noexcept = default;
//   ~Font() = default;
//
//   /**
//    * @brief Creates a font using a custom em size.
//    *
//    * If loading fails, a fallback empty font is returned.
//    *
//    * @param fontFilename Path to the font file.
//    * @param emSize        Target glyph scale in pixels.
//    * @return Font instance.
//    */
//   static std::optional<Font> create(const char *fontFilename, double emSize);
//
//   /** Returns the GPU atlas texture (read-only). */
//   const inline Texture &getAtlasTexture() const { return m_atlasTexture; }
//
//   /** Returns the GPU atlas texture (mutable). */
//   inline Texture &getAtlasTexture() { return m_atlasTexture; }
//
//   /** Returns the font geometry describing metrics and layout. */
//   const inline msdf_atlas::FontGeometry &getFontGeometry() const
//   {
//     return m_fontGeometry;
//   }
//
//   /** Returns the generated glyph geometry array. */
//   const inline std::vector<msdf_atlas::GlyphGeometry> &getGlyphGemoetry()
//   const
//   {
//     return m_glyphs;
//   }
//
// private:
//   //
//   ---------------------------------------------------------------------------
//   // Data
//   //
//   ---------------------------------------------------------------------------
//
//   /** Font metrics and glyph layout data. */
//   msdf_atlas::FontGeometry m_fontGeometry;
//   /** Geometry information for each glyph in the atlas. */
//   std::vector<msdf_atlas::GlyphGeometry> m_glyphs;
//   /** GPU texture storing the MSDF atlas. */
//   Texture m_atlasTexture;
//
//   //
//   ---------------------------------------------------------------------------
//   // Internal Methods
//   //
//   ---------------------------------------------------------------------------
//
//   /**
//    * @brief Generates an MSDF atlas texture from a font file.
//    *
//    * @param fontFilename Path to the font file.
//    * @param emSize        Target glyph scale.
//    * @return Generated atlas texture.
//    */
//   Texture generateAtlas(const char *fontFilename, double emSize);
//
//   /** Builds the default Latin character set used for glyph loading. */
//   static msdf_atlas::Charset getLatinCharset();
//
//   /**
//    * @brief Converts generated MSDF atlas data into a GPU texture.
//    */
//   template <typename T, typename S, int N,
//             msdf_atlas::GeneratorFunction<S, N> GenFunc>
//   Texture
//   createAtlasTexture(const std::vector<msdf_atlas::GlyphGeometry> &glyphs,
//                      int width, int height, const char *fontFilename);
//
//   /** Constructs a font and immediately generates its atlas. */
//   Font(const char *fontFilename, double emSize);
// };
//
// } // namespace pain
