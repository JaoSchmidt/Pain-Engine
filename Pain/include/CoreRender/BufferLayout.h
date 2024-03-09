#pragma once
#include "LogWrapper.h"
#include <cstdint>
#include <string>
#include <vector>

namespace pain
{
enum class ShaderDataType {
  None = 0,
  Float,
  Float2,
  Float3,
  Float4,
  Mat3,
  Mat4,
  Int,
  Int2,
  Int3,
  Int4,
  Bool
};

struct BufferElement {

  ShaderDataType type;
  uint32_t size;
  uint32_t offset;
  bool normalized;

  BufferElement() = default;
  BufferElement(ShaderDataType type, const std::string &name,
                bool normalized = false)
      : type(type), size(getComponentSize()), offset(0), normalized(normalized)
  {
  }
  constexpr uint32_t getComponentCount() const
  {
    constexpr uint32_t counts[] = {
        0,     // None
        1,     // Float
        2,     // Float2
        3,     // Float3
        4,     // Float4
        3 * 3, // Mat3
        4 * 4, // Mat4
        1,     // Int
        2,     // Int2
        3,     // Int3
        4,     // Int4
        1      // Bool
    };
    static_assert(static_cast<uint32_t>(ShaderDataType::Bool) <
                      sizeof(counts) / sizeof(counts[0]),
                  "Missing entry in counts array");

    return counts[static_cast<uint32_t>(type)];
  }
  constexpr uint32_t getComponentSize() const
  {
    constexpr uint32_t sizes[] = {
        0,         // None
        4,         // Float
        4 * 2,     // Float2
        4 * 3,     // Float3
        4 * 4,     // Float4
        4 * 3 * 3, // Mat3
        4 * 4 * 4, // Mat4
        4,         // Int
        4 * 2,     // Int2
        4 * 3,     // Int3
        4 * 4,     // Int4
        1          // Bool
    };

    static_assert(static_cast<uint32_t>(ShaderDataType::Bool) <
                      sizeof(sizes) / sizeof(sizes[0]),
                  "Missing entry in sizes array");

    return sizes[static_cast<uint32_t>(type)];
  }
  constexpr GLenum getComponentGLType() const
  {
    constexpr GLenum types[] = {
        GL_FLOAT, // NONE
        GL_FLOAT, // Float
        GL_FLOAT, // Float2
        GL_FLOAT, // Float3
        GL_FLOAT, // Float4
        GL_FLOAT, // Mat3
        GL_FLOAT, // Mat4
        GL_INT,   // Int
        GL_INT,   // Int2
        GL_INT,   // Int3
        GL_INT,   // Int4
        GL_BOOL   // Bool
    };

    static_assert(static_cast<GLenum>(ShaderDataType::Bool) <
                      sizeof(types) / sizeof(types[0]),
                  "Missing entry in types array");

    return types[static_cast<uint32_t>(type)];
  }
};

class BufferLayout
{
public:
  BufferLayout() {}

  BufferLayout(const std::initializer_list<BufferElement> &elements)
      : m_Elements(elements)
  {
    CalculateOffsetsAndStride();
  }

  inline uint32_t GetStride() const { return m_Stride; }
  inline const std::vector<BufferElement> &GetElements() const
  {
    return m_Elements;
  }

  inline std::vector<BufferElement>::iterator begin()
  {
    return m_Elements.begin();
  }
  inline std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
  inline std::vector<BufferElement>::const_iterator begin() const
  {
    return m_Elements.begin();
  }
  inline std::vector<BufferElement>::const_iterator end() const
  {
    return m_Elements.end();
  }

private:
  void CalculateOffsetsAndStride()
  {
    uint32_t o_offset = 0;
    m_Stride = 0;
    for (auto &element : m_Elements) {
      element.offset = o_offset;
      o_offset += element.size;
      m_Stride += element.size;
    }
  }
  std::vector<BufferElement> m_Elements;
  uint32_t m_Stride = 0;
};

} // namespace pain
