#include "CoreRender/Text/TextureFont.h"
#include "CoreFiles/LogWrapper.h"
#include "freetype/freetype.h"

#define HRESf 64.f

namespace pain
{

void TextureFont::genAllKerning(FT_Library *library, FT_Face *face)
{
  FT_UInt glyphIndex, leftIndex;
  TextureGlyph *glyph, *leftGlyph;
  FT_Vector kerning;

  for (size_t i = 1; i < m_glyphs.size(); i++) {
    glyph = &m_glyphs[i];
    glyphIndex = FT_Get_Char_Index(*face, glyph->m_codePoint);

    for (size_t j = 1; j < m_glyphs.size(); j++) {
      leftGlyph = &m_glyphs[j];
      leftIndex = FT_Get_Char_Index(*face, leftGlyph->m_codePoint);

      // FT_KERNING_UNFITTED returns FT_F26Dot6 values.
      FT_Get_Kerning(*face, leftIndex, glyphIndex, FT_KERNING_UNFITTED,
                     &kerning);
      if (kerning.x) {
        glyph->setKerning(leftGlyph->m_codePoint,
                          TextureGlyph::convertF26Dot6ToFloat(kerning.x) /
                              HRESf);
      }

      // PLOG_I("kerning {}({})-{}({}): {}", glyph->m_codePoint, glyphIndex,
      //        leftGlyph->m_codePoint, leftIndex, kerning.x);

      // kerning function isn't commutative
      FT_Get_Kerning(*face, glyphIndex, leftIndex, FT_KERNING_UNFITTED,
                     &kerning);
      if (kerning.x) {
        leftGlyph->setKerning(glyph->m_codePoint,
                              kerning.x / (float)(HRESf * HRESf));
      }
    }
  }
}

} // namespace pain
