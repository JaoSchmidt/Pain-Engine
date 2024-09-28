#pragma once

#include "pch.h"

#include "CoreRender/Text/TextureAtlas.h"
#include "CoreRender/Text/TextureGlyph.h"

namespace pain
{
struct TextureFont {
  TextureFont(); // equi. to texture_font_init
  // Load texture font from a file
  TextureFont(const TextureAtlas *atlas, const float point_size,
              const char *filename);
  // Load texture font from memory
  TextureFont(const TextureAtlas *atlas, const float point_size,
              const void *memoryBase, size_t memorySize);
  TextureFont *clone(const TextureFont &other, float pointSize);
  ~TextureFont();

  std::vector<TextureGlyph> m_glyphs;
  const TextureAtlas *m_textTexture = nullptr; // equi. "atlas" in source
  FontLocation m_fontLocation;                 // equi. "location" in source
  union {
    const char *m_filename; // for when font is on a file
    struct {                // for when font is inside the memory
      const void *m_memoryBase;
      size_t m_memorySize;
    };
  };
  FontMode m_libraryFontMode;   // equi to "font_library_t.mode"
  FT_Library m_freeTypeLibrary; // equil. to "font_library_t.library"

  float m_fontSize;                 // equi. size in source
  RenderMode m_nextGlyphRenderMode; // equi. renderMode in source
  float m_outlineThickness;
  bool m_useLcdFilter; // equi. "filtering" in source
  bool m_useKerning;   // equi. "kerning" in source
  bool m_useAutohint;  // equi. "hinting" in source
  bool m_scaleTexture; // equi. "scaletext" in source
  std::array<unsigned char, 5> m_lcdWeights;

  // face (https://freetype.org/freetype2/docs/glyphs/glyphs-3.html)
  float m_lineGap;          // a constant
  float m_ascender;         // highest character coordinate
  float m_descender;        // lowest character coordinate
  float m_baselineDistance; // m_ascender - m_descender + m_lineGap (i.e
                            // height)
  float m_faceUnderlinePosition;
  float m_faceUnderlineThickness;
  int padding;
  FontMode m_fontMode; // equi. to "mode" in source
  FT_Face m_ftFace;    // equi. to "face" in source
  FT_Size m_ftSize;    // equi. to "ft_size" in source
  float m_coordScale;  // equi. to "scale" in source

  bool doesFontHasColor(); // equi. to "texture_is_color_font"
  // Scale font size or select the best font size in case font has fixed sizes
  bool setFontSize(float fontSize); // equi. to texture_font_set_size()
  void initSize();
  bool hasMultipleMasters(); // equi. to texture_font_is_variable

private:
  TextureFont(const TextureFont &other);
  void close(FontMode faceMode, FontMode libraryMode);
  void initLibrary();
  void init();
  void setFreeTypeLibrary(void);
  void closeFTLibrary(FontMode faceMode, FontMode libraryMode);
  bool loadFace(float size);
  bool getWeight(FT_Fixed *defaultWeight, FT_Fixed *minWeight,
                 FT_Fixed *maxWeight);
  bool setWeight(FT_Fixed newWeight);

  // ---------------------------------------------------- //
  // glyphs
  // ---------------------------------------------------- //
  TextureGlyph &getGlyph(const char *codePoint);
  TextureGlyph &addGlyph(const char *codePoint);
  bool hasGlyph(const char *codePoint);
  bool delGlyph(const char *codePoint);
  bool loadGlyph(const char *codePoint);
  bool loadMultipleGlyphs(const char *codePoints);
  bool isLoaded(const char *codePoint);

  // index glyphs
  bool indexGlyph(const TextureGlyph &glyph,
                  uint32_t codePoint); // return true if glyph was copied,
                                       // false if inserted
  TextureGlyph &getGlyph(const uint32_t glyphIndex);
  bool loadGlyph(uint32_t glyphIndex);

  void enlargeAtlas(size_t newWidth, size_t newHeight);
  void scaleGlyphs(size_t widthScaleFactor, size_t heightScaleFactor);

  // ---------------------------------------------------- //
  // Kerning: The spacing between two letters
  // (https://freetype.org/freetype2/docs/glyphs/glyphs-4.html)
  // ---------------------------------------------------- //

  float getKerning(const char *codePoint);
  // Iterates through all glyphs and sets their kernings
  void genAllKerning(FT_Library *library, FT_Face *face);

  // ---------------------------------------------------- //
  // Converters
  // ---------------------------------------------------- //
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
