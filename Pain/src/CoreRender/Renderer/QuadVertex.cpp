#include "CoreRender/Renderer/QuadVertex.h"

namespace pain
{
void QuadVertexBatch::goBackToFirstQuad()
{
  m_indexCount = 0;
  m_vertexBufferPtr = m_vertexBufferBase;
}
void QuadVertexBatch::sendAllDataToOpenGL()
{
  uint32_t dataSize =
      (uint8_t *)m_vertexBufferPtr - (uint8_t *)m_vertexBufferBase;
  m_vertexBuffer->setData((void *)m_vertexBufferBase, dataSize);
}

/* Batch logic mostly in this function */
void QuadVertexBatch::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                               const glm::vec4 &color)
{
  m_vertexBufferPtr->Position = {position.x, position.y, 0.0f};
  m_vertexBufferPtr->Color = color;
  m_vertexBufferPtr->TexCoord = {0.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y, 0.0f};
  m_vertexBufferPtr->Color = color;
  m_vertexBufferPtr->TexCoord = {1.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y + size.y,
                                 0.0f};
  m_vertexBufferPtr->Color = color;
  m_vertexBufferPtr->TexCoord = {1.0f, 1.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x, position.y + size.y, 0.0f};
  m_vertexBufferPtr->Color = color;
  m_vertexBufferPtr->TexCoord = {0.0f, 1.0f};
  m_vertexBufferPtr++;

  m_indexCount += 6;
}

QuadVertexBatch::QuadVertexBatch()
{
  m_vertexArray.reset(new VertexArray());

  m_vertexBuffer.reset(new VertexBuffer(MaxVertices * sizeof(QuadVertex)));
  m_vertexBuffer->setLayout({
      {ShaderDataType::Float3, "a_Position"}, //
      {ShaderDataType::Float4, "a_Color"},    //
      {ShaderDataType::Float2, "a_TexCoord"}  //
  });
  m_vertexArray->addVertexBuffer(m_vertexBuffer);

  m_vertexBufferBase = new QuadVertex[MaxVertices];

  uint32_t *quadIndices = new uint32_t[MaxIndices];

  uint32_t offset = 0;
  for (uint32_t i = 0; i < MaxIndices; i += 6) {
    quadIndices[i + 0] = offset + 0;
    quadIndices[i + 1] = offset + 1;
    quadIndices[i + 2] = offset + 2;

    quadIndices[i + 3] = offset + 2;
    quadIndices[i + 4] = offset + 3;
    quadIndices[i + 5] = offset + 0;

    offset += 4;
  }

  std::shared_ptr<IndexBuffer> quadIB;
  quadIB.reset(new IndexBuffer(quadIndices, MaxIndices));
  m_vertexArray->setIndexBuffer(quadIB);
  delete[] quadIndices;

  m_whiteTexture.reset(new Texture(1, 1));
  // uint32_t whiteTextureData = 0xffffffff;
  // m_whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

  m_textureShader.reset(new Shader("resources/shaders/Texture.glsl"));
  // m_textureShader->bind();
  // m_textureShader->uploadUniformInt("u_Texture", 0);
}

} // namespace pain
