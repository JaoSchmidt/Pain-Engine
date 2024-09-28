#pragma once

#include "pch.h"
#include <vector>
/**
 * Will generate a giant texture with all possibe text characters
 * There is a thousand ways to store all glyphs inside a single texture
 * This will cover a reasonable approach for merging them,
 * to render basic stuff
 */

namespace pain
{

/**
 * This class will merge several small regions into a single texture.
 */
class TextureAtlas
{
public:
  // Creates a new texture of width x height with depth "depth"
  TextureAtlas(const size_t width, const size_t height, const size_t depth);
  ~TextureAtlas();
  // Allocates a region of width x height
  glm::ivec4 getTextRegion(const size_t width, const size_t height);
  // Fill region with some data
  void setTextRegion(const size_t regionX, const size_t regionY,
                     const size_t reagionWidth, const size_t regionHeight,
                     const unsigned char *uploadableData, const size_t stride);
  void clearAllRegions();
  void enlargeTexture(const size_t width_new, const size_t height_new);

  void *m_specialGlyph;

private:
  void createSpecialGlyph();
  int fitTexture(const size_t index, const size_t width, const size_t height);
  void mergeTexture();
  std::vector<glm::ivec3> m_nodes;
  glm::uvec2 m_textureSize;
  size_t m_textureDepth; // Depth (in bytes) of the giant texture
  size_t m_used;
  unsigned int m_charID;
  unsigned char *m_TextureData;
  bool m_isModified;
};

} // namespace pain
