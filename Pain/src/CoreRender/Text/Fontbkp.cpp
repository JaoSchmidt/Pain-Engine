// /*
//  * This Source Code Form is subject to the terms of the Mozilla Public
//  * License, v. 2.0. If a copy of the MPL was not distributed with this
//  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  */
//
// #include "CoreRender/Text/Font.h"
// #include "CoreFiles/AppConstants.h"
//
// #include "CoreFiles/LogWrapper.h"
//
// namespace pain
// {
//
// std::optional<Font> Font::create(const char *fontFilename, double emSize)
// {
//   try {
//     return Font(fontFilename, emSize);
//   } catch (const std::exception &e) {
//     PLOG_W("Defaulting to default font texture");
//     return {};
//   }
// }
//
// Font::Font(const char *fontFilename, double emSize)
//     : m_atlasTexture(generateAtlas(fontFilename, emSize)) {};
//
// Texture Font::generateAtlas(const char *fontFilename, double emSize)
// {
//   // Initialize instance of FreeType library
//   msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
//   P_ASSERT(ft, "Could not load FreeType library");
//   // Load font file
//   msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilename);
//   if (font == nullptr) {
//     PLOG_W("Font file not found \"{}\"", fontFilename);
//     throw std::runtime_error(std::string("Font file not found \"") +
//                              std::string(fontFilename) + std::string("\""));
//   }
//   // Storage for glyph geometry and their coordinates in the atlas
//   m_fontGeometry = msdf_atlas::FontGeometry(&m_glyphs);
//
//   constexpr double fontScale = 1.0;
//   msdf_atlas::Charset charset = getLatinCharset();
//   int numOfLoadedGlyphs = m_fontGeometry.loadCharset(font, fontScale,
//   charset);
//
//   PLOG_I("Loaded {} glyphs from font {}, out of {}", numOfLoadedGlyphs,
//          fontFilename, charset.size());
//
//   // Apply MSDF edge coloring.
//   const double maxCornerAngle = 3.0;
//   for (msdf_atlas::GlyphGeometry &glyph : m_glyphs)
//     glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
//   // TightAtlasPacker class computes the layout of the atlas.
//   msdf_atlas::TightAtlasPacker packer;
//   packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
//   packer.setMinimumScale(24.0);
//   packer.setPixelRange(2.0);
//   packer.setMiterLimit(1.0);
//   packer.setScale(emSize);
//   packer.pack(m_glyphs.data(), (int)m_glyphs.size());
//   // Get final atlas dimensions
//   int width = 0, height = 0;
//   packer.getDimensions(width, height);
//   // The ImmediateAtlasGenerator class facilitates the generation of the
//   // atlas bitmap.
//   Texture texture =
//       createAtlasTexture<uint8_t, float, 3, msdf_atlas::msdfGenerator>(
//           m_glyphs, width, height, fontFilename);
//   // Cleanup
//   msdfgen::destroyFont(font);
//   msdfgen::deinitializeFreetype(ft);
//   return texture;
// }
// template <typename T, typename S, int N,
//           msdf_atlas::GeneratorFunction<S, N> GenFunc>
// Texture
// Font::createAtlasTexture(const std::vector<msdf_atlas::GlyphGeometry>
// &glyphs,
//                          const int width, const int height,
//                          const char *fontFilename)
// {
//   msdf_atlas::GeneratorAttributes attributes;
//   attributes.config.overlapSupport = true;
//   attributes.scanlinePass = true;
//
//   msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc,
//                                       msdf_atlas::BitmapAtlasStorage<T, N>>
//       generator(width, height);
//   generator.setAttributes(attributes);
//   generator.setThreadCount(AppConstants::getProcessorCount() / 2);
//   generator.generate(glyphs.data(), (int)glyphs.size());
//
//   msdfgen::BitmapConstRef<T, N> bitmap =
//       (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();
//
//   Texture texture = *Texture::createTexture(fontFilename, bitmap.width,
//                                             bitmap.height,
//                                             ImageFormat::RGB8);
//   texture.setData((void *)bitmap.pixels, bitmap.width * bitmap.height * 3);
//
//   return texture;
// }
//
// msdf_atlas::Charset Font::getLatinCharset()
// {
//   struct CharsetRange {
//     uint32_t Begin, End;
//   };
//   static constexpr CharsetRange charsetRanges[] = {{0x0020, 0x00FF}};
//   msdf_atlas::Charset charset;
//
//   for (CharsetRange range : charsetRanges) {
//     for (uint32_t c = range.Begin; c <= range.End; c++)
//       charset.add(c);
//   }
//   return charset;
// }
//
// } // namespace pain
