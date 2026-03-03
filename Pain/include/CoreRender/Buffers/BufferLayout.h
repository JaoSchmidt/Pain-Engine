/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// BufferLayout.h
#pragma once

#include "Core.h"
#include "pch.h"

namespace pain
{

/**
 * @enum ShaderDataType
 * @brief **GPU shader attribute data types** used by vertex buffers.
 *
 * ---
 * ## **Purpose**
 * Defines the supported element formats that can be sent to the GPU.
 * Each type maps directly to a byte size and component layout used when
 * building vertex buffer layouts.
 *
 * ---
 * ## **Usage**
 * Typically used when defining a `BufferLayout`:
 * ```
 * { ShaderDataType::Float3, "a_Position" }
 * { ShaderDataType::UByte4, "a_Color", true }
 * ```
 */
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

/**
 * @struct BufferElement
 * @brief **Single vertex attribute description** inside a buffer layout.
 *
 * ---
 * ## **Represents**
 * One attribute in a vertex buffer (position, color, UVs, etc).
 * Stores type information, byte size, offset, normalization state,
 * and the attribute name used by shaders.
 */
struct BufferElement {

  ShaderDataType type;
  std::string name;
  bool normalized = false;
  bool instanced = false;
  uint32_t size;
  uint32_t offset;
  /** Returns the attribute name used in the shader. */
  std::string const &getName() { return name; }

  BufferElement() = default;
  /**
   * @brief Creates a buffer element definition.
   *
   * @param type        Attribute data type.
   * @param name        Shader attribute name.
   * @param normalized  Whether the data should be normalized by the GPU.
   */
  BufferElement(ShaderDataType type, const std::string &name,
                bool normalized = false, bool instanced = false)
      : type(type), name(name), normalized(normalized), instanced(instanced),
        size(getComponentSize()), offset(0) {};
  MOVABLE(BufferElement);
  COPYABLE(BufferElement);
  ~BufferElement() = default;
  /**
   * @brief Returns the **byte size** of this element type.
   *
   * ---
   * ## **Notes**
   * - Size is derived directly from the underlying `ShaderDataType`.
   * - Used internally when calculating stride and offsets.
   */
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

/**
 * @class BufferLayout
 * @brief **Describes the memory layout of a vertex buffer.**
 *
 * ---
 * ## **Purpose**
 * BufferLayout defines how vertex attributes are packed in memory:
 * - Attribute ordering
 * - Byte offsets
 * - Total stride per vertex
 *
 * This information is consumed by the renderer when configuring
 * vertex array bindings.
 *
 * ---
 * ## **Example**
 * ```
 * BufferLayout layout = {
 *   { ShaderDataType::Float3, "a_Position" },
 *   { ShaderDataType::UByte4, "a_Color", true },
 *   { ShaderDataType::Float2, "a_TexCoord" }
 * };
 * ```
 */
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

  /**
   * @brief Constructs a layout from a list of buffer elements.
   *
   * Automatically computes byte offsets and total stride.
   *
   * @param elements Initializer list of buffer elements.
   */
  BufferLayout(const std::initializer_list<BufferElement> &elements)
      : m_Elements(elements)
  {
    calculateOffsetsAndStride();
  }

  /** Returns the total stride (in bytes) of a single vertex. */
  inline uint32_t getStride() const { return m_Stride; }
  /** Returns the ordered list of buffer elements. */
  inline const std::vector<BufferElement> &getElements() const
  {
    return m_Elements;
  }
  /** Iterator access to buffer elements. */
  inline std::vector<BufferElement>::iterator begin()
  {
    return m_Elements.begin();
  }
  /** Iterator access to buffer elements. */
  inline std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
  /** Const iterator access to buffer elements. */
  inline std::vector<BufferElement>::const_iterator begin() const
  {
    return m_Elements.begin();
  }
  /** Const iterator access to buffer elements. */
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
