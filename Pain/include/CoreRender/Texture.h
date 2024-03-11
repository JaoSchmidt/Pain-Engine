#include "pch.gch"

#include "Core.h"

namespace pain
{

class EXPORT Texture
{
public:
  Texture(const std::string &path);
  ~Texture();
  void bind(uint32_t slot = 0) const;

private:
  std::string m_path;
  uint32_t m_width, m_height;
  uint32_t m_rendererId;
};

} // namespace pain
