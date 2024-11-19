#pragma once

#include "CoreRender/Texture.h"
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <vector>

namespace pain
{
class Font
{
public:
  Font(const char *fontFilename);
  const inline Texture &getAtlasTexture() const { return m_atlasTexture; };
  const inline msdf_atlas::FontGeometry &getFontGeometry() const
  {
    return m_fontGeometry;
  };
  const inline std::vector<msdf_atlas::GlyphGeometry> &getGlyphGemoetry() const
  {
    return m_glyphs;
  };
  static Font *getDefault();

private:
  // variables
  msdf_atlas::FontGeometry m_fontGeometry;
  std::vector<msdf_atlas::GlyphGeometry> m_glyphs;
  Texture m_atlasTexture;
  static Font *m_defaultFont;

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
