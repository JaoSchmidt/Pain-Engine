/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** FrameBuffer.h */
#pragma once

#include "Core.h"
#include "CoreRender/Texture.h"

namespace pain
{

/**
 * @struct FrameBufferCreationInfo
 * @brief **Configuration used to create a framebuffer.**
 */
struct FrameBufferCreationInfo {
  /** When true, the framebuffer represents the swapchain (screen). */
  bool swapChainTarget = false;
  /** Framebuffer width in pixels. */
  int32_t width = 800;
  /** Framebuffer height in pixels. */
  int32_t height = 600;
  /** MSAA sample count. */
  uint32_t samples = 1;
  /** Renderer texture ID used as color attachment. */
  uint32_t colorAttachmentTextureId = 0;
  /** Renderer texture ID used as depth attachment. */
  uint32_t depthAttachmentTextureId = 0;
  /** Backend framebuffer handle. */
  uint32_t bufferId = 0;
};

/**
 * @class FrameBuffer
 * @brief **GPU framebuffer abstraction.**
 *
 * Wraps a backend framebuffer resource and manages its lifetime,
 * resizing, and binding.
 */
class FrameBuffer
{
public:
  /**
   * Creates a framebuffer from the given specification.
   * @return A valid framebuffer on success, or std::nullopt on failure.
   */
  static std::optional<FrameBuffer> create(const FrameBufferCreationInfo &info);

  /** Resizes the framebuffer attachments. */
  void resizeFrameBuffer(int32_t width, int32_t height);

  /** Binds the framebuffer for rendering. */
  void bind();

  /** Unbinds the currently bound framebuffer. */
  void unbind();

  FrameBuffer(const FrameBufferCreationInfo &spec) : m(spec) {}
  MOVABLE(FrameBuffer);
  NONCOPYABLE(FrameBuffer);

  /** Returns the framebuffer creation specification. */
  const FrameBufferCreationInfo &getSpecification() const { return m; }

  /** Returns the renderer ID of the color attachment texture. */
  const uint32_t &getColorAttachmentRendererId() const
  {
    return m.colorAttachmentTextureId;
  }

  /** Width accessors. */
  uint32_t getWidth() const { return static_cast<uint32_t>(m.width); }
  float getWidthf() const { return static_cast<float>(m.width); }
  int32_t getWidthi() const { return static_cast<int32_t>(m.width); }

  /** Height accessors. */
  uint32_t getHeight() const { return static_cast<uint32_t>(m.height); }
  float getHeightf() const { return static_cast<float>(m.height); }
  int32_t getHeighti() const { return static_cast<int32_t>(m.height); }

private:
  FrameBufferCreationInfo m;
};

} // namespace pain
