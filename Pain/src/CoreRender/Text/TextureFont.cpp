#include "CoreRender/Text/TextureFont.h"
#include "Core.h"
#include "CoreFiles/LogWrapper.h"
#include "freetype/fttypes.h"

#include <cstdlib>
#include <cstring>
#include <ft2build.h>
#include FT_TRUETYPE_TABLES_H
#include FT_FREETYPE_H
#include FT_SIZES_H
#include FT_STROKER_H
// #include FT_ADVANCES_H
#include FT_LCD_FILTER_H
#include FT_MULTIPLE_MASTERS_H

#define HRESf 64.f
#define HRES 64
#define DPI 72

#define DEFAULT_MODE MODE_FREE_CLOSE

namespace pain
{

void TextureFont::init()
{
  m_glyphs = {};
  m_baselineDistance = 0.f;
  m_descender = 0.f;
  m_lineGap = 0.f;
  m_nextGlyphRenderMode = RENDER_NORMAL;
  m_outlineThickness = 0.f;
  m_useAutohint = true;
  m_useKerning = true;
  m_useLcdFilter = true;
  m_scaleTexture = true;
  m_coordScale = 1.f;

  // FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
  // FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
  m_lcdWeights = {0x10, 0x40, 0x70, 0x40, 0x10};

  // the source code returned -1 here, instead of giving an error.
  if (loadFace(m_fontSize))
    PLOG_E("Unable to load freetype face");

  initSize();
  if (setFontSize(m_fontSize))
    PLOG_E("Unable to set freetype font size");

  /* NULL is a special glyph */
  getGlyph(nullptr);
}

TextureFont::TextureFont() { init(); }

TextureFont::TextureFont(const TextureAtlas *atlas, const float point_size,
                         const char *filename)
    : m_textTexture(atlas), m_fontSize(point_size)
{
  P_ASSERT(filename, "Invalid filename \"{}\"", filename);
  m_fontLocation = TEXTURE_FONT_FILE;
  m_filename = filename;
  m_fontMode = DEFAULT_MODE;
  init();
}

TextureFont::TextureFont(const TextureAtlas *atlas, const float point_size,
                         const void *memoryBase, size_t memorySize)
{
  P_ASSERT(memoryBase, "Invalid memory base");
  P_ASSERT(memorySize > 0, "Invalid memory size");
  m_fontLocation = TEXTURE_FONT_FILE;
  m_memoryBase = memoryBase;
  m_memorySize = memorySize;
  m_fontMode = DEFAULT_MODE;
  init();
}

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
                          convertF26Dot6ToFloat(kerning.x) / HRESf);
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

bool TextureFont::doesFontHasColor()
{
  // Color BitMap Data Table (CBDT). If it has no data, then it has no color
  static const uint32_t tag = FT_MAKE_TAG('C', 'B', 'D', 'T');
  unsigned long length = 0;
  FT_Load_Sfnt_Table(m_ftFace, tag, 0, NULL, &length);
  return length != 0;
}

bool TextureFont::setFontSize(float fontSize)
{
  FT_Error error = 0;
  // clang-format off
  FT_Matrix matrix = {
    (int)((1.0 / HRES) * 0x10000L),
    (int)((0.0) * 0x10000L),
    (int)((0.0) * 0x10000L),
    (int)((1.0) * 0x10000L)
  }; // clang-format on

  if (FT_HAS_FIXED_SIZES(m_ftFace)) {
    /* Algorithm to select best size */
    P_ASSERT_W(m_ftFace->num_fixed_sizes != 0, "No fixed size in color font");

    int bestMatch = 0;
    float diff = 1e20;
    for (int i = 0; i < m_ftFace->num_fixed_sizes; i++) {
      float newSize = convertF26Dot6ToFloat(m_ftFace->available_sizes[i].size);
      float ndiff =
          fontSize > newSize ? fontSize / newSize : newSize / fontSize;

      PLOG_I("Candidate: size[{}]={} {}*{}", i, newSize,
             m_ftFace->available_sizes[i].width,
             m_ftFace->available_sizes[i].height);

      if (ndiff < diff) {
        bestMatch = i;
        diff = ndiff;
      }
    }
    PLOG_I("Selected: size[{}] for {}", bestMatch, fontSize);
    error = FT_Select_Size(m_ftFace, bestMatch);
    if (error) {
      PLOG_E("FreeType error: {}", FT_Error_String(error));
      return 0;
    }
    m_scaleTexture =
        m_fontSize /
        convertF26Dot6ToFloat(m_ftFace->available_sizes[bestMatch].size);

  } else {
    error = FT_Set_Char_Size(m_ftFace, convertF26Dot6ToFloat(fontSize), 0,
                             DPI * HRES, DPI);
    if (error) {
      PLOG_E("FreeType error: {}", FT_Error_String(error));
      return 0;
    }
  }
  FT_Set_Transform(m_ftFace, &matrix, nullptr);
  return 1;
}

void TextureFont::initSize()
{
  FT_Size_Metrics metrics;

  m_faceUnderlinePosition =
      m_ftFace->underline_position / (float)(HRESf * HRESf) * m_fontSize;
  m_faceUnderlinePosition = roundf(m_faceUnderlinePosition);
  if (m_faceUnderlinePosition > -2) {
    m_faceUnderlinePosition = -2.0;
  }

  m_faceUnderlineThickness =
      m_ftFace->underline_thickness / (float)(HRESf * HRESf) * m_fontSize;
  m_faceUnderlineThickness = roundf(m_faceUnderlineThickness);
  if (m_faceUnderlineThickness < 1) {
    m_faceUnderlineThickness = 1.0;
  }

  // see (https://freetype.org/freetype2/docs/glyphs/glyphs-3.html)
  metrics = m_ftFace->size->metrics;
  m_ascender = metrics.ascender >> 6;
  m_descender = metrics.descender >> 6;
  m_baselineDistance = metrics.height >> 6;
  m_lineGap = m_baselineDistance - m_ascender + m_descender;

  m_libraryFontMode = MODE_ALWAYS_OPEN;
}

// clang-format off
TextureFont::TextureFont(const TextureFont &other)
    : m_glyphs(other.m_glyphs),
      m_textTexture(other.m_textTexture), // copy reference
      m_fontLocation(other.m_fontLocation),
      m_libraryFontMode(other.m_libraryFontMode),
      // FT_Library: Copy the handle (assuming shallow copy, managed externally)
      m_freeTypeLibrary(other.m_freeTypeLibrary),
      m_fontSize(other.m_fontSize),
      m_nextGlyphRenderMode(other.m_nextGlyphRenderMode),
      m_outlineThickness(other.m_outlineThickness),
      m_useLcdFilter(other.m_useLcdFilter),
      m_useKerning(other.m_useKerning),
      m_useAutohint(other.m_useAutohint),
      m_scaleTexture(other.m_scaleTexture),
      m_lcdWeights(other.m_lcdWeights),
      m_lineGap(other.m_lineGap),
      m_ascender(other.m_ascender),
      m_descender(other.m_descender),
      m_baselineDistance(other.m_baselineDistance),
      m_faceUnderlinePosition(other.m_faceUnderlinePosition),
      m_faceUnderlineThickness(other.m_faceUnderlineThickness),
      padding(other.padding),
      m_fontMode(other.m_fontMode),
      m_ftFace(other.m_ftFace), // Assuming FreeType face can be shared
                                // (otherwise, clone the face)
      m_ftSize(other.m_ftSize), // Same as above
      m_coordScale(other.m_coordScale)
{
  // clang-format on
  if (other.m_fontLocation == TEXTURE_FONT_FILE) {
    if (other.m_filename) {
      m_filename = strdup(other.m_filename);
    } else {
      m_filename = nullptr;
    }
  } else {
    m_memoryBase = malloc(other.m_memorySize);
    if (m_memoryBase) {
      memcpy((void *)m_memoryBase, other.m_memoryBase, other.m_memorySize);
    }
    m_memorySize = other.m_memorySize;
  }
}

TextureFont *TextureFont::clone(const TextureFont &other, float pointSize)
{
  TextureFont *self = new TextureFont();
  *self = other; // using copy constructor
  self->m_fontSize = pointSize;

  FT_Error error = FT_New_Size(self->m_ftFace, &self->m_ftSize);
  if (error) {
    PLOG_W("Unable to get freetype new size");
    delete self;
    return nullptr;
  }
  error = FT_Activate_Size(self->m_ftSize);
  if (error) {
    PLOG_W("Unable to activate freetype new size");
    delete self;
    return nullptr;
  }

  // Set the font size (this function should handle FreeType size setting)
  if (!self->setFontSize(pointSize)) {
    PLOG_W("Unable to set Font Size");
    delete self;
    return nullptr;
  }

  // equi. to `texture_font_init_size` in original source
  self->initSize();

  // Adjust the glyph vector if the size/scale ratio doesn't match the native
  // size
  float native_size =
      other.m_fontSize / other.m_coordScale; // Unscaled size in original font
  if (self->m_fontSize / self->m_coordScale != native_size) {
    self->m_glyphs.clear(); // Clear the glyphs and reset the vector (or
                            // reallocate if necessary)
  }
  return self; // Return the new clone
}

void TextureFont::initLibrary()
{
  FT_Error error = FT_Init_FreeType(&m_freeTypeLibrary);
  P_ASSERT(error != 0, "Error initiating FreeType library");
  m_libraryFontMode = MODE_ALWAYS_OPEN;
}

void TextureFont::close(FontMode faceMode, FontMode libraryMode)
{
  if (m_ftFace && m_fontMode <= faceMode) {
    FT_Done_Face(m_ftFace);
    m_ftFace = nullptr;
  } else {
    return; // never close the library when the face stays open
  }
  if (m_freeTypeLibrary && m_libraryFontMode <= libraryMode) {
    FT_Done_Face(m_ftFace);
    m_freeTypeLibrary = nullptr;
  }
}

bool TextureFont::loadFace(float size)
{
  FT_Error error = 0;

  if (m_freeTypeLibrary == nullptr) {
    initLibrary();
  }
  if (m_ftFace == nullptr) {
    switch (m_fontLocation) {
    case TEXTURE_FONT_FILE:
      error = FT_New_Face(m_freeTypeLibrary, m_filename, 0, &m_ftFace);
      if (error) {

        PLOG_E("Error loading face from filename");
        close(MODE_ALWAYS_OPEN, MODE_ALWAYS_OPEN);
      }
      break;
    case TEXTURE_FONT_MEMORY:
      error = FT_New_Memory_Face(m_freeTypeLibrary, m_memoryBase, m_memorySize,
                                 0, &m_ftFace);
      if (error) {
        PLOG_E("Error loading face from memory");
        close(MODE_ALWAYS_OPEN, MODE_ALWAYS_OPEN);
      }
      break;
    }

    error = FT_Select_Charmap(m_ftFace, FT_ENCODING_UNICODE);
    if (error) {
      PLOG_E("Error selecting map of chars from the current Face");
      close(MODE_ALWAYS_OPEN, MODE_FREE_CLOSE);
      return 0;
    }
    error = FT_New_Size(m_ftFace, &m_ftSize);
    if (error) {
      PLOG_E("Error setting new size for Face");
      close(MODE_ALWAYS_OPEN, MODE_FREE_CLOSE);
      return 0;
    }
    error = FT_Activate_Size(m_ftSize);
    if (error) {
      PLOG_E("Error activating new size for Face");
      close(MODE_ALWAYS_OPEN, MODE_FREE_CLOSE);
      return 0;
    }
    if (!setFontSize(size)) {
      PLOG_E("Error setting the font size");
      close(MODE_ALWAYS_OPEN, MODE_FREE_CLOSE);
      return 0;
    }
  }
  return 1;
}

bool TextureFont::hasMultipleMasters()
{
  int result = 0;
  if (m_ftFace)
    result = m_ftFace->face_flags & FT_FACE_FLAG_MULTIPLE_MASTERS;
  return result;
}

bool TextureFont::getWeight(FT_Fixed *def, FT_Fixed *min, FT_Fixed *max)
{
  int result = 0;
  if (def && min && max) {
    *def = 0;
    *min = 0;
    *max = 0;
    if (m_freeTypeLibrary != nullptr && m_ftFace) {
      FT_MM_Var *master;
      if (FT_Get_MM_Var(m_ftFace, &master) == 0) {
        const FT_Tag tag = FT_MAKE_TAG(
            'w', 'g', 'h', 't'); // read about sfnt format and its tags
        const char *name = "Weight";
        // tries to find the weight axis (i.e "wght" tag)
        for (unsigned int i = 0; i < 16 && i < master->num_axis; i++) {
          if (tag == master->axis[i].tag ||
              strcmp(name, master->axis[i].name) == 0) {
            *def = master->axis[i].def;
            *min = master->axis[i].minimum;
            *max = master->axis[i].maximum;
            result = 1;
            break;
          }
        }
        FT_Done_MM_Var(m_freeTypeLibrary, master);
      }
    }
  }
  return result;
}

bool TextureFont::setWeight(FT_Fixed wght)
{
  int result = 0;
  if (m_freeTypeLibrary && m_ftFace) {
    FT_MM_Var *master;
    if (FT_Get_MM_Var(m_ftFace, &master) == 0) {
      const FT_Tag tag = FT_MAKE_TAG('w', 'g', 'h', 't');
      const char *name = "Weight";
      // tries to find the weight axis (i.e "wght" tag)
      for (unsigned int i = 0; i < 16 && i < master->num_axis; i++) {
        if (tag == master->axis[i].tag ||
            strcmp(name, master->axis[i].name) == 0) {
          const FT_Fixed min = master->axis[i].minimum;
          const FT_Fixed max = master->axis[i].maximum;
          if (wght >= min && wght <= max) {
            const int n = i + 1;
            FT_Fixed coords[16];
            if (FT_Get_Var_Design_Coordinates(m_ftFace, n, coords) == 0) {
              coords[i] = wght;

              if (FT_Set_Var_Design_Coordinates(m_ftFace, n, coords) == 0)
                result = 1;
            }
          } else {
            result = -1;
          }
          break;
        }
      }
      FT_Done_MM_Var(m_freeTypeLibrary, master);
    }
  }

  if (result < 0) {
    PLOG_E("Variable Font Weight is out of range");
  } else if (result == 0) {
    PLOG_E("Variable Font Weight not available");
  }
  return result == 1;
}

TextureFont::~TextureFont()
{
  P_ASSERT(m_ftFace, "The object must be valid before deleting resources");
  FT_Error error = FT_Done_Size(m_ftSize);
  if (error) {
    PLOG_E("Freetype error code {}: {}", error, FT_Error_String(error));
  }
  close(MODE_ALWAYS_OPEN, MODE_FREE_CLOSE);

  if (m_fontLocation == TEXTURE_FONT_FILE && m_filename)
    delete m_filename;

  m_glyphs.clear();
}

TextureGlyph &TextureFont::getGlyph(const char *codepoint)
{
  if (!codepoint) {
    return *(TextureGlyph *)m_textTexture->m_specialGlyph;
  }
  return
}

} // namespace pain
