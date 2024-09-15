#include "CoreRender/Text/TextureGlyph.h"
#include <vector>

namespace pain
{
TextureGlyph::TextureGlyph()
{
  m_codePoint = -1;
  m_size = {0, 0};
  /* Attributes that can have different images for the same codepoint */
  m_renderMode = RENDER_NORMAL;
  m_outlineThickness = 0.0;
  m_glyphMode = GLYPH_END;

  /* End of attribute part */
  m_bearing = {0, 0};
  m_advanceX = 0.0;
  m_advanceY = 0.0;
  m_textureCoord = {0.f, 0.f};
  m_textureSize = {0.f, 0.f};
  m_kerning = {};
}

void TextureGlyph::textureFontDefaultMode(FontMode fontMode)
{
  // TODO: Read about __THREAD
}

float TextureGlyph::getKerning(const char *codepoint)
{
  if (codepoint == nullptr)
    return 0;
  uint32_t ucodepoint = convertUtf8ToUtf32(codepoint);
  uint32_t i = ucodepoint >> 8;
  uint32_t j = ucodepoint & 0xFF;
  if (i > m_kerning.size())
    return 0;
  return m_kerning[i][j];
}
void TextureGlyph::setKerning(uint32_t codepoint, float kerning)
{
  uint32_t i = codepoint >> 8;
  uint32_t j = codepoint & 0xFF;
  if (i <= m_kerning.size())
    m_kerning[i][j] = kerning;
}

} // namespace pain
