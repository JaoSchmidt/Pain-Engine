/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "Core.h"
#include "pch.h"

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
  Bool,
  UByte,
  UByte2,
  UByte3,
  UByte4,
};

struct BufferElement {

  ShaderDataType type;
  uint32_t size;
  uint32_t offset;
  bool normalized;
  std::string name;
  std::string const &getName() { return name; }

  BufferElement() = default;
  BufferElement(ShaderDataType type, const std::string &name,
                bool normalized = false)
      : type(type), size(getComponentSize()), offset(0), normalized(normalized),
        name(name) {};
  MOVABLE(BufferElement);
  COPYABLE(BufferElement);
  ~BufferElement() = default;

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
        1,         // Bool
        1,         // UByte
        2,         // UByte2
        3,         // UByte3
        4          // UByte4
    };

    static_assert(static_cast<uint32_t>(ShaderDataType::Bool) <
                      sizeof(sizes) / sizeof(sizes[0]),
                  "Missing entry in sizes array");

    return sizes[static_cast<uint32_t>(type)];
  }
};

class BufferLayout
{
public:
  BufferLayout() {}
  NONCOPYABLE(BufferLayout);
  BufferLayout(BufferLayout &&o)
      : m_Elements(o.m_Elements), m_Stride(o.m_Stride) {};
  BufferLayout &operator=(BufferLayout &&o)
  {
    if (this != &o) {
      m_Elements = o.m_Elements;
      m_Stride = o.m_Stride;
    }
    return *this;
  }

  BufferLayout(const std::initializer_list<BufferElement> &elements)
      : m_Elements(elements)
  {
    calculateOffsetsAndStride();
  }

  inline uint32_t getStride() const { return m_Stride; }
  inline const std::vector<BufferElement> &getElements() const
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
  void calculateOffsetsAndStride()
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
