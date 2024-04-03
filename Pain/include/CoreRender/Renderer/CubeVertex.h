#pragma once
#include "pch.h"

#include "Core.h"

#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"

namespace pain
{

struct CubeVertex {
  glm::vec3 Position;
  glm::vec4 Color;
  glm::vec2 TexCoord;
};

class EXPORT CubeVertexBatch
{
public:
  CubeVertexBatch();
  const std::shared_ptr<VertexArray> &getVertexArray() const
  {
    return m_vertexArray;
  };
  const uint32_t &getIndexCount() const { return m_indexCount; };
  void goBackToFirst();
  void sendAllDataToOpenGL();
  void drawBatch(const glm::vec3 &position, const glm::vec3 &size,
                 const glm::vec4 &color);
  inline const std::shared_ptr<Shader> getShader() const
  {
    return m_textureShader;
  };

private:
  const uint32_t MaxCubes = 1000;
  const uint32_t MaxVertices = MaxCubes * 8;
  const uint32_t MaxIndices = MaxCubes * 36;

  std::shared_ptr<VertexArray> m_vertexArray;
  std::shared_ptr<VertexBuffer> m_vertexBuffer;
  std::shared_ptr<Shader> m_textureShader;
  std::shared_ptr<Texture> m_whiteTexture;

  CubeVertex *m_vertexBufferBase = nullptr;
  CubeVertex *m_vertexBufferPtr = nullptr;
  uint32_t m_indexCount = 0;
};

} // namespace pain
