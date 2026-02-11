/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** @file Buffers.h */
#pragma once

#include "Core.h"
#include "CoreRender/BufferLayout.h"
#include "pch.h"

namespace pain
{

/**
 * @class VertexBuffer
 * @brief **GPU vertex buffer abstraction.**
 *
 * ---
 * ## **Purpose**
 * Encapsulates a backend vertex buffer resource and its associated
 * `BufferLayout`. Supports both *dynamic* and *static* buffers.
 *
 * The buffer owns the backend resource and releases it automatically
 * on destruction.
 */
class VertexBuffer
{
public:
  /**
   * @brief Creates a **dynamic vertex buffer**.
   *
   * ---
   * ## **Usage**
   * Intended for buffers whose contents change frequently.
   *
   * @param size   Buffer size in bytes.
   * @param layout Vertex layout description.
   * @return Optional vertex buffer on success.
   */
  static std::optional<VertexBuffer> createVertexBuffer(uint32_t size,
                                                        BufferLayout &&layout);

  /**
   * @brief Creates a **static vertex buffer** with initial data.
   *
   * ---
   * ## **Usage**
   * Intended for immutable or rarely updated geometry.
   *
   * @param vertices Pointer to vertex data.
   * @param size     Data size in bytes.
   * @param layout   Vertex layout description.
   * @return Optional vertex buffer on success.
   */
  static std::optional<VertexBuffer>
  createStaticVertexBuffer(float *vertices, uint32_t size,
                           BufferLayout &&layout);

  VertexBuffer(VertexBuffer &&o);
  VertexBuffer &operator=(VertexBuffer &&o);
  NONCOPYABLE(VertexBuffer)
  ~VertexBuffer();

  /** Binds this vertex buffer to the current graphics pipeline. */
  void bind() const;

  /** Unbinds the currently bound vertex buffer. */
  void unbind() const;

  /** Returns the backend (OpenGL/Vulkan/Directx/etc) buffer identifier. */
  uint32_t getId() const { return m_bufferId; }

  /** Returns the buffer layout associated with this buffer. */
  const BufferLayout &getLayout() const { return m_layout; }

  /**
   * @brief Updates the contents of the buffer.
   *
   * ---
   * ## **Notes**
   * Valid only for buffers created as *dynamic*.
   *
   * @param data Pointer to source data.
   * @param size Size in bytes.
   */
  void setData(const void *data, uint32_t size);

private:
  VertexBuffer(uint32_t bufferId, BufferLayout &&layout);

  uint32_t m_bufferId;
  BufferLayout m_layout;
};

/**
 * @class IndexBuffer
 * @brief **GPU index buffer abstraction.**
 *
 * ---
 * ## **Purpose**
 * Stores index data used for indexed rendering.
 * Manages the lifetime of the backend index buffer resource.
 */
class IndexBuffer
{
public:
  /**
   * @brief Creates an index buffer from index data.
   *
   * @param indexes Pointer to index array.
   * @param count   Number of indices.
   * @return Optional index buffer on success.
   */
  static std::optional<IndexBuffer> createIndexBuffer(uint32_t *indexes,
                                                      uint32_t count);
  /**
   * @brief Creates a dynamic index buffer
   *
   * @param maxIndexCount Max number of indices
   * @return Optional index buffer on success.
   */
  static std::optional<IndexBuffer> createIndexBuffer(uint32_t maxIndexCount);

  IndexBuffer(IndexBuffer &&o);
  IndexBuffer &operator=(IndexBuffer &&o);
  NONCOPYABLE(IndexBuffer);
  ~IndexBuffer();

  /** Binds this index buffer to the current graphics pipeline. */
  void bind() const;

  /** Unbinds the currently bound index buffer. */
  void unbind() const;

  /** Returns the number of indices stored in the buffer. */
  uint32_t getCount() const { return m_count; };

  void updateIndexBuffer(const uint32_t *indices, uint64_t count);

private:
  IndexBuffer(uint32_t bufferId, uint32_t count);

  uint32_t m_bufferId;
  uint32_t m_count;
};

} // namespace pain
