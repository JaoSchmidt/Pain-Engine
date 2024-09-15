#pragma once

#include "pch.h"

#include "CoreRender/Text/TextSheetTexture.h"
#include "CoreRender/Text/TextureGlyph.h"

namespace pain
{
struct TextureFont {
  std::vector<TextureGlyph> m_glyphs;
  TextTextureGen m_textTexture;
  FontLocation m_fontLocation;
  union {
    char *filename; // for when font is on a file
    struct {        // for when font is inside the memory
      const void *base;
      size_t size;
    };
  };
  FontMode m_libraryFontMode;
  FT_Library m_freeTypeLibrary;

  float m_fontSize;
  RenderMode m_nextGlyphRenderMode;
  float m_outlineThickness;
  bool m_useLcdFilter;
  bool m_useKerning;
  bool m_useAutohint;
  bool m_scaleTexture;
  bool m_lcdWeights[5];

  // face (https://freetype.org/freetype2/docs/glyphs/glyphs-3.html)
  float m_lineGap;          // a constant
  float m_ascender;         // highest character coordinate
  float m_descender;        // lowest character coordinate
  float m_baselineDistance; // m_ascender - m_descender + m_lineGap
  float m_faceUnderlinePosition;
  float m_faceUnderlineThickness;
  int padding;
  FontMode m_fontMode;
  FT_Face m_ftFace;
  FT_Size m_ftSize;
  float m_coordScale;

private:
  void setFreeTypeLibrary(void);
  TextureFont(const TextTextureGen &ttg, const float point_size,
              const char &filename);
  // TextureFont(const TextTextureGen *textTextureGen);
  // ~TextureFont();
  void closeFTLibrary(FontMode faceMode, FontMode libraryMode);
  int loadFace(float size);
  bool hasMultipleMasters();
  bool getWeight(FT_Fixed *defaultWeight, FT_Fixed *minWeight,
                 FT_Fixed *maxWeight);
  bool setWeight(FT_Fixed newWeight);
  // glyphs
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

  // kerning
  float getKerning(const char *codePoint);
  // Iterates through all glyphs and sets their kernings
  void genAllKerning(FT_Library *library, FT_Face *face);
};

} // namespace pain
