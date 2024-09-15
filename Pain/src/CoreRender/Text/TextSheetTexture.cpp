#include "CoreRender/Text/TextSheetTexture.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Text/TextureGlyph.h"
#include <cstring>

namespace pain
{

void TextTextureGen::createSpecialGlyph()
{
  glm::vec4 region = getTextRegion(5, 5);
  TextureGlyph *glyph = new TextureGlyph();
  // fully black glyph
  constexpr unsigned char data[4 * 4 * 3] = {
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

  P_ASSERT_W(region.x >= 0, "Texture Atlas Full");

  setTextRegion(region.x, region.y, 4, 4, data, 0);
  glyph->m_unicodePoint = -1;
  glyph->m_textureCoord = {region.x + 2 / (float)m_textureSize.x,
                           region.y + 2 / (float)m_textureSize.y};
  glyph->m_textureSize = {region.x + 3 / (float)m_textureSize.x,
                          region.y + 3 / (float)m_textureSize.y};
  m_specialGlyph = (void *)glyph;
}

TextTextureGen::TextTextureGen(const size_t width, const size_t height,
                               const size_t depth)
{
  P_ASSERT_W(depth == 1 || depth == 3 || depth == 4,
             "Depth must be equal to 1,3 or 4. Current Depth: {}", depth);
  m_nodes = {{1, 1, width - 2}};
  m_used = 0;
  m_textureSize = {width, height};
  m_textureDepth = depth;
  m_charID = 0;
  m_isModified = true;
  m_TextureData = new unsigned char[width * height * depth];
}

TextTextureGen::~TextTextureGen() { delete m_TextureData; }

void TextTextureGen::setTextRegion(const size_t x, const size_t y,
                                   const size_t width, const size_t height,
                                   const unsigned char *data,
                                   const size_t stride)
{
  P_ASSERT(x > 0 && y > 0 && x + width <= m_textureSize.x - 1 &&
               y + height <= m_textureSize.y - 1,
           "Region size must be contained inside Sheet Texture");
  P_ASSERT(height == 0 || (data != nullptr && width > 0),
           "Preventing copying data from undefined position")

  size_t i;
  const size_t depth = m_textureDepth;
  const size_t charsize = sizeof(char);
  for (i = 0; i < height; i++) {
    std::memcpy(m_TextureData +
                    ((y + i) * m_textureSize.x + x) * charsize * depth,
                data + (i * stride) * charsize, width * charsize * depth);
  }
  m_isModified = true;
}
int TextTextureGen::fitTexture(const size_t index, const size_t width,
                               const size_t height)
{
  glm::ivec3 &node = m_nodes[index];
  if ((node.x + width) > (m_textureSize.x - 1)) {
    return -1;
  }

  int y = node.y;
  size_t i = index;
  size_t width_remaning = width;
  while (width_remaning > 0) {
    glm::ivec3 &node = m_nodes[i];
    if (node.y > y) {
      y = node.y;
    }
    if ((y + height) > (m_textureSize.y - 1)) {
      return -1;
    }
    width_remaning -= node.z;
    ++i;
  }
  return y;
}

void TextTextureGen::mergeTexture()
{
  size_t i;

  for (i = 0; i < m_nodes.size() - 1; ++i) {
    glm::ivec3 &node = m_nodes[i];
    glm::ivec3 &next = m_nodes[i + 1];
    if (node.y == next.y) {
      node.z += next.z;
      m_nodes.erase(m_nodes.begin() + i + 1);
      --i;
    }
  }
}

glm::ivec4 TextTextureGen::getTextRegion(const size_t width,
                                         const size_t height)
{
  int y, best_index;
  size_t best_height, best_width;
  glm::ivec4 region = {0, 0, width, height};
  size_t i;

  best_height = UINT_MAX;
  best_index = -1;
  best_width = UINT_MAX;

  for (i = 0; i < m_nodes.size(); ++i) {
    y = fitTexture(i, width, height);
    if (y >= 0) {
      glm::ivec3 &node = m_nodes[i];
      if (((y + height) < best_height) ||
          (((y + height) == best_height) &&
           (node.z > 0 && (size_t)node.z < best_width))) {
        best_height = y + height;
        best_index = i;
        best_width = node.z;
        region.x = node.x;
        region.y = y;
      }
    }
  }

  if (best_index == -1) {
    region.x = -1;
    region.y = -1;
    region.b = 0;
    region.a = 0;
    return region;
  }

  glm::ivec3 node = {0, 0, 0};
  node.x = region.x;
  node.y = region.y + height;
  node.z = width;
  m_nodes[best_index] = node;

  for (i = best_index + 1; i < m_nodes.size(); ++i) {
    glm::ivec3 &node = m_nodes[i];
    glm::ivec3 &prev = m_nodes[i - 1];

    if (node.x < (prev.x + prev.z)) {
      int shrink = prev.x + prev.z - node.x;
      node.x += shrink;
      node.z -= shrink;
      if (node.z <= 0) {
        m_nodes.erase(m_nodes.begin() + i);
        --i;
      } else {
        break;
      }
    } else {
      break;
    }
  }
  mergeTexture();
  m_used += width * height;
  m_isModified = true;
  return region;
}

void TextTextureGen::clearAllRegions()
{
  glm::ivec3 node = {1, 1, 1};
  m_nodes.clear();
  m_used = 0;
  node.z = m_textureSize.x - 2;
  m_nodes.push_back(node);
  std::memset(m_TextureData, 0,
              m_textureSize.x * m_textureSize.y * m_textureDepth);
}

void TextTextureGen::enlargeTexture(size_t width_new, size_t height_new)
{
  // ensure size increased
  P_ASSERT(width_new >= m_textureSize.x,
           "New texture must be bigger than the old one");
  P_ASSERT(height_new >= m_textureSize.y,
           "New texture must be bigger than the old one");
  P_ASSERT(width_new + height_new > m_textureSize.x + m_textureSize.y,
           "New texture must be bigger than the old one");

  size_t width_old = m_textureSize.x;
  size_t height_old = m_textureSize.y;
  // allocate new buffer
  unsigned char *data_old = m_TextureData;
  m_TextureData = new unsigned char[width_new * width_old * m_textureDepth];
  // update atlas size
  m_textureSize = {width_new, height_new};
  // add node reflecting the gained space on the right
  if (width_new > width_old) {
    glm::ivec3 node;
    node.x = width_old - 1;
    node.y = 1;
    node.z = width_new - width_old;
    m_nodes.push_back(node);
  }
  // copy over data from the old buffer, skipping first row and column because
  // of the margin
  size_t pixel_size = sizeof(char) * m_textureDepth;
  size_t old_row_size = width_old * pixel_size;
  setTextRegion(1, 1, width_old - 2, height_old - 2,
                data_old + old_row_size + pixel_size, old_row_size);
  delete data_old;
}

} // namespace pain
