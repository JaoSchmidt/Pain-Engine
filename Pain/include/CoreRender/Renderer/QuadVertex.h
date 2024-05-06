#pragma once
#include "pch.h"

#include "Core.h"

#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"

namespace pain
{

struct QuadVertex {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 texCoord;
  float texIndex;
  float tilingFactor;
};

class EXPORT QuadVertexBatch
{
public:
  QuadVertexBatch();
  const std::shared_ptr<VertexArray> &getVertexArray() const
  {
    return m_vertexArray;
  };
  const uint32_t &getIndexCount() const { return m_indexCount; };
  void goBackToFirstQuad();
  void sendAllDataToOpenGL();
  inline const std::shared_ptr<Shader> getShader() const
  {
    return m_textureShader;
  };
  void bindTextures();

  // draw functions
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const glm::vec4 &color);
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const std::shared_ptr<Texture> &texture, float tilingFactor,
                glm::vec4 tintColor);

private:
  const uint32_t MaxQuads = 10000;
  const uint32_t MaxVertices = MaxQuads * 4;
  const uint32_t MaxIndices = MaxQuads * 6;
  static const uint32_t MaxTextureSlots =
      32; // TODO: dinamically see this value on gpu

  std::shared_ptr<VertexArray> m_vertexArray;
  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  std::shared_ptr<Shader> m_textureShader;
  std::shared_ptr<Texture> m_whiteTexture;
  std::array<std::shared_ptr<Texture>, MaxTextureSlots> m_textureSlots;

  QuadVertex *m_vertexBufferBase = nullptr;
  QuadVertex *m_vertexBufferPtr = nullptr;
  uint32_t m_indexCount = 0;
  uint32_t m_textureSlotIndex = 1;
};

} // namespace pain
