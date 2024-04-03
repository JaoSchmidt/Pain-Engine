#pragma once
#include "pch.h"

#include "Core.h"

#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"

namespace pain
{

struct QuadVertex {
  glm::vec3 Position;
  glm::vec4 Color;
  glm::vec2 TexCoord;
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
  void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                const glm::vec4 &color);
  inline const std::shared_ptr<Shader> getShader() const
  {
    return m_textureShader;
  };

private:
  const uint32_t MaxQuads = 10000;
  const uint32_t MaxVertices = MaxQuads * 4;
  const uint32_t MaxIndices = MaxQuads * 6;

  std::shared_ptr<VertexArray> m_vertexArray;
  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  std::shared_ptr<Shader> m_textureShader;
  std::shared_ptr<Texture> m_whiteTexture;

  QuadVertex *m_vertexBufferBase = nullptr;
  QuadVertex *m_vertexBufferPtr = nullptr;
  uint32_t m_indexCount = 0;
};

} // namespace pain
