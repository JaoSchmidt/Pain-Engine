#pragma once

#include "CoreRender/Texture.h"
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <vector>

namespace pain
{
class FontManager
{
public:
  FontManager(const char *fontFilename);
  inline Texture &getAtlasTexture() { return m_atlasTexture; };

private:
  // variables
  msdf_atlas::FontGeometry m_fontGeometry;
  std::vector<msdf_atlas::GlyphGeometry> m_glyphs;
  Texture m_atlasTexture;

  // consts
  const char *textureKey = "fontAltas";
  static constexpr double pixelRange = 2.0;
  static constexpr double glyphScale = 32.0;
  static constexpr double miterLimit = 1.0;
  static constexpr double maxCornerAngle = 3.0;

  // methods
  Texture generateAtlas(const char *fontFilename);
  msdf_atlas::Charset getLatinCharset();

  template <typename T, typename S, int N,
            msdf_atlas::GeneratorFunction<S, N> GenFunc>
  Texture
  createAtlasTexture(const char *fontName, float fontSize,
                     const std::vector<msdf_atlas::GlyphGeometry> &glyphs,
                     const msdf_atlas::FontGeometry &fontGeometry,
                     const float width, const float height);
};
} // namespace pain
