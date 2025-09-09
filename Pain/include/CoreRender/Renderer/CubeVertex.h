#pragma once
#include "pch.h"

#include "Core.h"

#include "Assets/DefaultTexture.h"
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

class CubeVertexBatch
{
public:
  CubeVertexBatch();
  const VertexArray &getVertexArray() const { return *m_vertexArray; };
  VertexArray &getVertexArray() { return *m_vertexArray; };
  const uint32_t &getIndexCount() const { return m_indexCount; };
  void goBackToFirst();
  void sendAllDataToOpenGL();
  void drawBatch(const glm::vec3 &position, const glm::vec3 &size,
                 const glm::vec4 &color);
  inline const Shader &getShader() const { return *m_textureShader; };
  inline Shader &getShader() { return *m_textureShader; };

private:
  const uint32_t MaxCubes = 1000;
  const uint32_t MaxVertices = MaxCubes * 8;
  const uint32_t MaxIndices = MaxCubes * 36;

  VertexArray *m_vertexArray;
  VertexBuffer *m_vertexBuffer;
  Shader *m_textureShader;
  Texture &m_whiteTexture =
      resources::getDefaultTexture(resources::DEFAULT_TEXTURE::BLANK);

  CubeVertex *m_vertexBufferBase = nullptr;
  CubeVertex *m_vertexBufferPtr = nullptr;
  uint32_t m_indexCount = 0;
};

} // namespace pain
