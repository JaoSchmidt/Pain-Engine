#pragma once
#include "CoreFiles/LogWrapper.h"
#include "glm/fwd.hpp"
#include "pch.h"

#include "Core.h"

#include "external/freetype-gl/texture-font.h"
#include <cstring>
#include <filesystem>
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
  Markup(const char *family, float size,                      //
         bool bold = false,                                   //
         bool italic = false,                                 //
         float spacing = 0.f,                                 //
         float gamma = 1.f,                                   //
         const glm::vec4 &foregroundColor = glm::vec4(1.f),   //
         const glm::vec4 &backgroundColor = glm::vec4(1.f),   //
         bool outline = false,                                //
         const glm::vec4 &outlineColor = glm::vec4(1.f),      //
         bool undeline = false,                               //
         const glm::vec4 &underlineColor = glm::vec4(1.f),    //
         bool overline = false,                               //
         const glm::vec4 &overlineColor = glm::vec4(1.f),     //
         bool strikethrough = false,                          //
         const glm::vec4 &strikethroughColor = glm::vec4(1.f) //
  );
  glm::vec4 m_foregroundColor;    // Text color.
  glm::vec4 m_backgroundColor;    // Background color.
  bool m_outline;                 // Whether outline is active.
  glm::vec4 m_outlineColor;       // Outline color.
  bool m_underline;               // Whether underline is active.
  glm::vec4 m_underlineColor;     // Underline color.
  bool m_overline;                // Whether overline is active.
  glm::vec4 m_overlineColor;      // Overline color.
  bool m_strikethrough;           // Whether strikethrough is active.
  glm::vec4 m_strikethroughColor; // Strikethrough color.
  float m_kerning;                // Spacing between letters.

  void setSize(float newSize);
  void setBold(bool turnBold);
  void setItalic(bool turnItalic);
  // void setGamma();

  bool operator==(const Markup &o) const;
  bool compare(const char *family, float size, bool bold, bool italic) const;

  // Pointer on the corresponding font (family/size/bold/italic)
  ftgl::texture_font_t *m_font;

private:
  COPIES(Markup);
  friend class FontManager;

  // to change, they require to restart texture_font_t
  std::string m_family; // "normal", "sans", "serif" or "monospace", etc;
  float m_size;         // Font size.
  bool m_bold;          // Whether text is bold.
  bool m_italic;        // Whether text is italic.
  float m_gamma; // Gamma correction. NOTE: Don't know if truly requires restart
};

class FontManager
{
public:
  static void init(size_t width, size_t height, size_t depth);
  static void destroy();
  static void deleteFontFamily(ftgl::texture_font_t &font);

  static ftgl::texture_font_t &getFromFileName(const char *filepath, float size,
                                               bool bold, bool italic);
  static ftgl::texture_font_t &getFromMarkup(const Markup &markup);
  static ftgl::texture_font_t &
  getFromDescription(const char *family, float size, bool bold, bool italic);

private:
  static bool compare(const ftgl::texture_font_t &font, const char *family,
                      float size, bool bold, bool italic);
  static bool compare(const ftgl::texture_font_t &font,
                      const std::filesystem::path file, float size, bool bold,
                      bool italic);

  static std::vector<ftgl::texture_font_t> m_fonts;
  static char *m_cache;
  static std::unique_ptr<ftgl::texture_atlas_t> m_atlas;

  const static std::filesystem::path m_defaultPath;
};

} // namespace pain
