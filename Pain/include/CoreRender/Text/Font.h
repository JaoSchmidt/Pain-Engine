#pragma once
#include "pch.h"

#include "Core.h"

#include "external/freetype-gl/texture-font.h"
#include "texture-font.h"
#include <vector>

namespace pain
{

// TODO: For stuff that are able to alter without issue, leave as it is,
// but for stuff that cannot be alter on the fly, there must be the
// proper "set" functions for those. For example:
// Modifiable on the fly: Outline thickness, kerning, rendering modes,
// transformations, hinting, and spacing.
// Cannot be modified without reloading the font: Font size, font family/style,
// encoding, and some hinting methods.

// Simple structure that describes text properties.
class Markup
{
public:
  glm::vec4 m_foreground_color;    // Text color.
  glm::vec4 m_background_color;    // Background color.
  bool m_outline;                  // Whether outline is active.
  glm::vec4 m_outline_color;       // Outline color.
  bool m_underline;                // Whether underline is active.
  glm::vec4 m_underline_color;     // Underline color.
  bool m_overline;                 // Whether overline is active.
  glm::vec4 m_overline_color;      // Overline color.
  bool m_strikethrough;            // Whether strikethrough is active.
  glm::vec4 m_strikethrough_color; // Strikethrough color.
  float m_kerning;                 // Spacing between letters.

  void setSize(float newSize);
  void setBold(float newSize);
  void setItalic(float newSize);
  void setGamma(float newSize);

private:
  // Pointer on the corresponding font (family/size/bold/italic)
  texture_font_t *font;

  // to change, they require to restart texture_font_t
  char *m_family; // "normal", "sans", "serif" or "monospace", etc;
  float m_size;   // Font size.
  bool m_bold;    // Whether text is bold.
  bool m_italic;  // Whether text is italic.
  float m_gamma;  // Gamma correction.
};

class FontManager
{
  ftgl::texture_atlas_t *m_atlas;
  std::vector<texture_font_t *> m_fonts;

  void getFromFileName(const char *filename, float size);
  void getFromDescription(const char *family, const float size, const int bold,
                          const int italic);
  // same as getFromDescription but uses markup variables
  void getFromMarkup(const Markup &markup);
  void init();
  void deleteFont();
}

} // namespace pain
