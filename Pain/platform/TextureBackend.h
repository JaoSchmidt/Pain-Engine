// TextureBackend.h
#pragma once
#include <cstdint>
#include <optional>
#include <string>

#include "CoreRender/ImageFormat.h"

namespace pain::backend
{

struct TextureCreateInfo {
  const char *debugName;
  uint32_t width;
  uint32_t height;
  ImageFormat format;
};

struct TextureFromFileInfo {
  const char *path;
  int width = 0;
  int height = 0;
  int channels = 0; // 3 = RGB, 4 = RGBA
  unsigned char *pixels = nullptr;
  bool clampToEdge;
};

uint32_t createTexture(const TextureCreateInfo &info);
uint32_t createTextureFromFile(const TextureFromFileInfo &info);

void destroyTexture(uint32_t id);

void bindTexture(uint32_t id);
void bindTextureToSlot(uint32_t id, uint32_t slot);

void setTextureData(uint32_t id, uint32_t width, uint32_t height,
                    ImageFormat dataFormat, const void *data, uint32_t size);

} // namespace pain::backend
