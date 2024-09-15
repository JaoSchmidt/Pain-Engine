#include "freetype/fttypes.h"
#include "pch.h"

#include <cuchar>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace pain
{
enum RenderMode {
  RENDER_NORMAL = 0,
  RENDER_OUTLINE_EDGE,
  RENDER_OUTLINE_POSITIVE,
  RENDER_OUTLINE_NEGATIVE,
  RENDER_SIGNED_DISTANCE_FIELD
};
enum GlyphMode { GLYPH_END = 0, GLYPH_CONT };
enum FontLocation { TEXTURE_FONT_FILE = 0, TEXTURE_FONT_MEMORY };
enum FontMode {
  MODE_AUTO_CLOSE = 0,
  MODE_GLYPHS_CLOSE,
  MODE_FREE_CLOSE,
  MODE_MANUAL_CLOSE,
  MODE_ALWAYS_OPEN
};

// See https://learnopengl.com/In-Practice/Text-Rendering for an image reference
struct TextureGlyph {
  uint32_t m_codePoint;
  glm::ivec2 m_size;
  glm::ivec2 m_bearing;
  int m_bearingX;
  int m_bearingY;
  int m_advanceX;
  int m_advanceY; // for vertical text only
  glm::vec2 m_textureCoord;
  glm::vec2 m_textureSize;
  float m_outlineThickness;
  RenderMode m_renderMode;
  GlyphMode m_glyphMode;
  std::vector<glm::vec3> m_kerning;

  void textureFontDefaultMode(FontMode fontMode);
  TextureGlyph();
  // ~TextureGlyph();
  // TextureGlyph(const TextureGlyph &TextureGlyph);

  // kerning (https://freetype.org/freetype2/docs/glyphs/glyphs-4.html)
  float getKerning(const char *codepoint);
  void setKerning(uint32_t codepoint, float kerning); // eq to index kerning
  void genKerning(FT_Library *library, FT_Face *face);

  inline static float convertF26Dot6ToFloat(FT_F26Dot6 value)
  {
    return ((float)value) / 64.f;
  }
  inline static FT_F26Dot6 convertFloatToF26Dot6(float value)
  {
    return (FT_F26Dot6)(value * 64.f);
  }
  inline static uint32_t convertUtf8ToUtf32(const char *ptr)
  {
    // https://en.cppreference.com/w/cpp/string/multibyte/mbrtoc32
    std::mbstate_t state{}; // zero-initialized to initial state
    char32_t c32;
    std::mbrtoc32(&c32, ptr, strlen(ptr), &state);
    return c32;
  }
};
} // namespace pain
