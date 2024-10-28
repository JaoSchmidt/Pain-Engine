#include "CoreRender/Text/Font.h"
#include "CoreFiles/LogWrapper.h"
#include <cstring>
#include <memory>

namespace pain
{

void Markup::setBold(bool turnBold)
{
  if (m_bold != turnBold)
    m_font = &FontManager::getFromDescription(m_family.c_str(), m_size,
                                              turnBold, m_italic);
}
void Markup::setItalic(bool turnItalic)
{
  if (m_italic != turnItalic)
    m_font = &FontManager::getFromDescription(m_family.c_str(), m_size, m_bold,
                                              turnItalic);
}

std::vector<ftgl::texture_font_t> m_fonts = {};
const std::filesystem::path FontManager::m_defaultPath =
    std::filesystem::path("resources") / std::filesystem::path("fonts");

ftgl::texture_font_t &FontManager::getFromFileName(const char *filename,
                                                   float size, bool bold,
                                                   bool italic)
{
  namespace fs = std::filesystem;
  // will check filepath, but also check "resources/fonts" folder
  const fs::path file = fs::path(filename);
  const fs::path filePath = m_defaultPath / file;
  const bool fe = fs::exists(file);
  const bool fpe = fs::exists(filePath);
  if (fe || fpe)
    PLOG_E("Neither filepath {} nor {} exist", file.string(),
           filePath.string());

  for (ftgl::texture_font_t &font : m_fonts) {
    if (compare(font, file, size, bold, italic) ||
        compare(font, filePath, size, bold, italic)) {
      return font;
    }
  }

  // first test "file" directly
  if (fe) {
    ftgl::texture_font_t *font = ftgl::texture_font_new_from_file(
        m_atlas.get(), size, file.string().c_str());
    if (font) {
      ftgl::texture_font_t *ptrFont = font;
      m_fonts.push_back(std::move(*font));
      texture_font_load_glyphs(ptrFont, m_cache);
      return *ptrFont;
    }
  } else if (fpe) {
    ftgl::texture_font_t *font = ftgl::texture_font_new_from_file(
        m_atlas.get(), size, filePath.string().c_str());
    if (font) {
      ftgl::texture_font_t *ptrFont = font;
      m_fonts.push_back(std::move(*font));
      texture_font_load_glyphs(ptrFont, m_cache);
      return *ptrFont;
    }
  }
  // TODO: replace with default font here later
  P_ASSERT(false, "Font was not found, perhaps missing font file?");
}

ftgl::texture_font_t &FontManager::getFromDescription(const char *family,
                                                      float size, bool bold,
                                                      bool italic)
{
  namespace fs = std::filesystem;
  const fs::path fontPath = m_defaultPath / fs::path(family);
  P_ASSERT(fs::exists(family) || fs::exists(fontPath),
           "Neither filepath {} nor {} exist", family, fontPath.string());
  for (ftgl::texture_font_t &font : m_fonts) {
    if (compare(font, family, size, bold, italic))
      return font;
  }
  // TODO: This won't compile in release mode, so later replce with default font
  P_ASSERT(false, "Font from family \"{}\", size {}, {} and {} not found",
           family, size, bold ? "bold" : "not bold",
           italic ? "italic" : "not italic");
}

ftgl::texture_font_t &FontManager::getFromMarkup(const Markup &m)
{
  return getFromDescription(m.m_family.c_str(), m.m_size, m.m_bold, m.m_italic);
}

bool FontManager::compare(const ftgl::texture_font_t &font, const char *family,
                          float size, bool bold, bool italic)
{
  constexpr int FT_STYLE_FLAG_ITALIC = (1 << 0);
  constexpr int FT_STYLE_FLAG_BOLD = (1 << 1);
  const long style_flags = ftgl::texture_font_get_sytle_flags(&font);
  const char *f = ftgl::texture_font_get_family(&font);

  return (strcmp(f, family) == 0) && (font.size == size) &&
         ((style_flags & FT_STYLE_FLAG_BOLD) != 0) == bold &&
         ((style_flags & FT_STYLE_FLAG_ITALIC) != 0) == italic;
}

bool FontManager::compare(const ftgl::texture_font_t &font,
                          std::filesystem::path file, float size, bool bold,
                          bool italic)
{
  constexpr int FT_STYLE_FLAG_ITALIC = (1 << 0);
  constexpr int FT_STYLE_FLAG_BOLD = (1 << 1);
  const long style_flags = ftgl::texture_font_get_sytle_flags(&font);

  return (std::filesystem::path(font.filename) == file) &&
         (font.size == size) &&
         ((style_flags & FT_STYLE_FLAG_BOLD) != 0) == bold &&
         ((style_flags & FT_STYLE_FLAG_ITALIC) != 0) == italic;
}

void FontManager::init(size_t width, size_t height, size_t depth)
{
  m_atlas.reset(ftgl::texture_atlas_new(width, height, depth));
  P_ASSERT(m_atlas, "Cannot create a texture atlas, out of memory");
  m_cache = strdup(" ");
}

void FontManager::deleteFontFamily(ftgl::texture_font_t &font)
{

  for (int i = 0; i < m_fonts.size(); i++) {
    // TODO: finish m_fonts deleteFontFamily
  }
  delete m_cache;
}

void FontManager::destroy()
{
  m_fonts.clear();
  delete m_cache;
}

bool Markup::operator==(const Markup &o) const
{
  return std::string(m_family) == std::string(o.m_family) //
         && m_size == o.m_size                            //
         && m_bold == o.m_bold                            //
         && m_italic == o.m_italic                        //
         && m_gamma == o.m_gamma;                         //
}

} // namespace pain
