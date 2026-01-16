/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "CoreRender/Text/Font.h"

#include "CoreFiles/Application.h"
#include "CoreFiles/LogWrapper.h"

namespace pain
{

Font *Font::create(const char *fontFilename)
{
  try {
    return new Font(fontFilename, 40.0);
  } catch (const std::exception &e) {
    return getDefault();
  }
}
Font *Font::create(const char *fontFilename, double emSize)
{
  try {
    return new Font(fontFilename, emSize);
  } catch (const std::exception &e) {
    PLOG_W("Defaulting to default font texture");
    return getDefault();
  }
}

Font::Font(const char *fontFilename, double emSize)
    : m_atlasTexture(generateAtlas(fontFilename, emSize))
{
}

Texture Font::generateAtlas(const char *fontFilename, double emSize)
{
  // Initialize instance of FreeType library
  msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
  P_ASSERT(ft, "Could not load FreeType library");
  // Load font file
  msdfgen::FontHandle *font = msdfgen::loadFont(ft, fontFilename);
  if (!font) {
    PLOG_W("Font file not found \"{}\"", fontFilename);
    throw std::runtime_error(std::string("Font file not found \"") +
                             std::string(fontFilename) + std::string("\""));
  }
  // Storage for glyph geometry and their coordinates in the atlas
  // FontGeometry is a helper class that loads a set of glyphs from a single
  // font. It can also be used to get additional font metrics, kerning
  // information, etc.
  m_fontGeometry = msdf_atlas::FontGeometry(&m_glyphs);
  // Load a set of character glyphs:
  // The second argument can be ignored unless you mix different font sizes
  // in one atlas. In the last argument, you can specify a charset other
  // than ASCII. To load specific glyph indices, use loadGlyphs instead.

  constexpr double fontScale = 1.0;
  msdf_atlas::Charset charset = getLatinCharset();
  // msdf_atlas::Charset charset = msdf_atlas::Charset::ASCII;
  int numOfLoadedGlyphs = m_fontGeometry.loadCharset(font, fontScale, charset);

  PLOG_I("Loaded {} glyphs from font {}, out of {}", numOfLoadedGlyphs,
         fontFilename, charset.size());

  // Apply MSDF edge coloring. See edge-coloring.h for other coloring
  // strategies.
  const double maxCornerAngle = 3.0;
  for (msdf_atlas::GlyphGeometry &glyph : m_glyphs)
    glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
  // TightAtlasPacker class computes the layout of the atlas.
  msdf_atlas::TightAtlasPacker packer;
  // Set atlas parameters:
  // setDimensions or setDimensionsConstraint to find the best value
  packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
  // setScale for a fixed size or setMinimumScale to use the largest that
  // fits
  packer.setMinimumScale(24.0);
  // setPixelRange or setUnitRange
  packer.setPixelRange(2.0);
  packer.setMiterLimit(1.0);
  packer.setScale(emSize);
  // Compute atlas layout - pack glyphs
  packer.pack(m_glyphs.data(), (int)m_glyphs.size());
  // Get final atlas dimensions
  int width = 0, height = 0;
  packer.getDimensions(width, height);
  // The ImmediateAtlasGenerator class facilitates the generation of the
  // atlas bitmap.
  Texture texture =
      createAtlasTexture<uint8_t, float, 3, msdf_atlas::msdfGenerator>(
          m_glyphs, width, height, fontFilename);
  // Cleanup
  msdfgen::destroyFont(font);
  msdfgen::deinitializeFreetype(ft);
  return texture;
}
template <typename T, typename S, int N,
          msdf_atlas::GeneratorFunction<S, N> GenFunc>
Texture createAtlasTexture(
    // const char *fontName,
    const std::vector<msdf_atlas::GlyphGeometry> &glyphs,
    // const msdf_atlas::FontGeometry &fontGeometry,
    const int width, const int height, const char *fontFilename)
{
  msdf_atlas::GeneratorAttributes attributes;
  attributes.config.overlapSupport = true;
  attributes.scanlinePass = true;

  // config generator and generate
  msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc,
                                      msdf_atlas::BitmapAtlasStorage<T, N>>
      generator(width, height);
  generator.setAttributes(attributes);
  generator.setThreadCount(Application::getProcessorCount() / 2);
  generator.generate(glyphs.data(), (int)glyphs.size());
  // TODO: savePng and IMG_SavePNG can correclty save atlas textures with png
  // format. However, the Texture ojbect for most fonts isn't being generated
  // correctly. That is there is probably some error with the setData texture
  // function

  // msdfgen::savePng(generator.atlasStorage(), "output.png");
  msdfgen::BitmapConstRef<T, N> bitmap =
      (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

  // Tranform the msdfgen bitmap into a Texture
  Texture texture = createTexture(fontFilename, bitmap.width, bitmap.height,
                                  ImageFormat::RGB8);
  texture.setData((void *)bitmap.pixels, bitmap.width * bitmap.height * 3);

  // For tests only, transform into a SDL_Surface
  // SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
  //     (void *)bitmap.pixels, bitmap.width, bitmap.height, 24, bitmap.width *
  //     3, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000);
  // IMG_SavePNG(surf, "surfaceOutput.png");
  // SDL_FreeSurface(surf);

  return texture;
}

msdf_atlas::Charset Font::getLatinCharset()
{
  struct CharsetRange {
    uint32_t Begin, End;
  };
  // From imgui_draw.cpp
  static constexpr CharsetRange charsetRanges[] = {{0x0020, 0x00FF}};
  msdf_atlas::Charset charset;

  for (CharsetRange range : charsetRanges) {
    for (uint32_t c = range.Begin; c <= range.End; c++)
      charset.add(c);
  }
  return charset;
}

Font *Font::getDefault()
{
  static Font m_defaultFont("resources/default/fonts/OpenSans-Regular.ttf",
                            40.0);
  return &m_defaultFont;
}

} // namespace pain
