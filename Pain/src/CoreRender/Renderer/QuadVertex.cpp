#include "CoreRender/Renderer/QuadVertex.h"
#include "CoreFiles/LogWrapper.h"
#include <unistd.h>

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

QuadVertexBatch::QuadVertexBatch()
{
  m_vertexArray.reset(new VertexArray());

  m_vertexBuffer.reset(new VertexBuffer(MaxVertices * sizeof(QuadVertex)));
  m_vertexBuffer->setLayout({{ShaderDataType::Float3, "a_Position"}, //
                             {ShaderDataType::Float4, "a_Color"},    //
                             {ShaderDataType::Float2, "a_TexCoord"}, //
                             {ShaderDataType::Float, "a_TexIndex"},  //
                             {ShaderDataType::Float, "a_TilingFactor"}});
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
  uint32_t whiteTextureData = 0xffffffff;
  m_whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

  int32_t samplers[MaxTextureSlots];
  for (uint32_t i = 0; i < MaxTextureSlots; i++) {
    samplers[i] = i;
  }

  m_textureShader.reset(new Shader("resources/shaders/Texture.glsl"));
  m_textureShader->bind();
  m_textureShader->uploadUniformIntArray("u_Textures", samplers,
                                         MaxTextureSlots);
  m_textureSlots[0] = m_whiteTexture;
}
void QuadVertexBatch::bindTextures()
{
  for (uint32_t i = 0; i < m_textureSlotIndex; i++)
    m_textureSlots[i]->bind(i);
}
void QuadVertexBatch::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                               const std::shared_ptr<Texture> &texture,
                               float tilingFactor, const glm::vec4 &tintColor)
{
  // constexpr glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
  const glm::vec4 color = tintColor;

  float textureIndex = 0.0f;
  // tries to get texture from the m_textureSlots
  for (uint32_t i = 1; i < m_textureSlotIndex; i++) {
    if (*m_textureSlots[i].get() == *texture.get()) {
      textureIndex = (float)i;
      break;
    }
  }

  // otherwise use it to allocate new texture
  if (textureIndex == 0.0f) {
    textureIndex = (float)m_textureSlotIndex;
    m_textureSlots[m_textureSlotIndex] = texture;
    m_textureSlotIndex++;
  }
  P_ASSERT_W(textureIndex != 0.0f,
             "Missing texture inside a drawQuad that requires textures");
  // PLOG_I("Texture being used {}, path:{}", textureIndex, texture->tempGet());
  m_vertexBufferPtr->position = {position.x, position.y, 0.0f};
  m_vertexBufferPtr->color = color;
  m_vertexBufferPtr->texCoord = {0.0f, 0.0f};
  m_vertexBufferPtr->texIndex = textureIndex;
  m_vertexBufferPtr->tilingFactor = tilingFactor;
  m_vertexBufferPtr++;

  m_vertexBufferPtr->position = {position.x + size.x, position.y, 0.0f};
  m_vertexBufferPtr->color = color;
  m_vertexBufferPtr->texCoord = {1.0f, 0.0f};
  m_vertexBufferPtr->texIndex = textureIndex;
  m_vertexBufferPtr->tilingFactor = tilingFactor;
  m_vertexBufferPtr++;

  m_vertexBufferPtr->position = {position.x + size.x, position.y + size.y,
                                 0.0f};
  m_vertexBufferPtr->color = color;
  m_vertexBufferPtr->texCoord = {1.0f, 1.0f};
  m_vertexBufferPtr->texIndex = textureIndex;
  m_vertexBufferPtr->tilingFactor = tilingFactor;
  m_vertexBufferPtr++;

  m_vertexBufferPtr->position = {position.x, position.y + size.y, 0.0f};
  m_vertexBufferPtr->color = color;
  m_vertexBufferPtr->texCoord = {0.0f, 1.0f};
  m_vertexBufferPtr->texIndex = textureIndex;
  m_vertexBufferPtr->tilingFactor = tilingFactor;
  m_vertexBufferPtr++;

  m_indexCount += 6;
}

/* Batch logic mostly in this function */
void QuadVertexBatch::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                               const glm::vec4 &color)
{
  const float texIndex = 0.0f; // White Texture
  const float tilingFactor = 1.0f;

  m_vertexBufferPtr->position = {position.x, position.y, 0.0f};
  m_vertexBufferPtr->color = color;
  m_vertexBufferPtr->texCoord = {0.0f, 0.0f};
  m_vertexBufferPtr->texIndex = texIndex;
  m_vertexBufferPtr->tilingFactor = tilingFactor;
  m_vertexBufferPtr++;

  m_vertexBufferPtr->position = {position.x + size.x, position.y, 0.0f};
  m_vertexBufferPtr->color = color;
  m_vertexBufferPtr->texCoord = {1.0f, 0.0f};
  m_vertexBufferPtr->texIndex = texIndex;
  m_vertexBufferPtr->tilingFactor = tilingFactor;
  m_vertexBufferPtr++;

  m_vertexBufferPtr->position = {position.x + size.x, position.y + size.y,
                                 0.0f};
  m_vertexBufferPtr->color = color;
  m_vertexBufferPtr->texCoord = {1.0f, 1.0f};
  m_vertexBufferPtr->texIndex = texIndex;
  m_vertexBufferPtr->tilingFactor = tilingFactor;
  m_vertexBufferPtr++;

  m_vertexBufferPtr->position = {position.x, position.y + size.y, 0.0f};
  m_vertexBufferPtr->color = color;
  m_vertexBufferPtr->texCoord = {0.0f, 1.0f};
  m_vertexBufferPtr->texIndex = texIndex;
  m_vertexBufferPtr->tilingFactor = tilingFactor;
  m_vertexBufferPtr++;

  m_indexCount += 6;
}

} // namespace pain
