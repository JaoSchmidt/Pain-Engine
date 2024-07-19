#pragma once

#include "pch.h"

#include "Core.h"

namespace pain
{

class EXPORT Texture
{
public:
  Texture(const std::string &path);
  Texture(uint32_t width, uint32_t height);
  Texture(Texture &t);
  ~Texture();
  void bind(uint32_t slot = 0) const;
  void setData(void *data, uint32_t size);
  bool operator==(const Texture &other) const;

  std::string tempGet() const { return m_path; }

private:
  std::string m_path;
  uint32_t m_width, m_height;
  uint32_t m_dataFormat;
  uint32_t m_internalFormat;
  uint32_t m_rendererId;
};

} // namespace pain
