#include "CoreRender/Renderer/QuadVertex.h"
#include "CoreFiles/LogWrapper.h"

#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unistd.h>
namespace pain
{
void VertexBatch::goBackToFirstVertex()
{
  m_quadIndexCount = 0;
  m_quadVertexBufferPtr = m_quadVertexBufferBase;

  m_triIndexCount = 0;
  m_triVertexBufferPtr = m_triVertexBufferBase;
}

void VertexBatch::sendAllDataToOpenGL()
{
  // uint32_t dataSize =
  //     (uint8_t *)m_quadVertexBufferPtr - (uint8_t *)m_quadVertexBufferBase;
  // m_quadVertexBuffer->setData((void *)m_quadVertexBufferBase, dataSize);

  // dataSize = (uint8_t *)m_triVertexBufferPtr - (uint8_t
  // *)m_triVertexBufferBase; m_triVertexBuffer->setData((void
  // *)m_triVertexBufferBase, dataSize);
}

void VertexBatch::drawBatch(const glm::mat4 &viewProjectionMatrix)
{
  if (m_triIndexCount) {
    m_triVertexArray->bind();
    const uint32_t triDataSize =
        (uint8_t *)m_triVertexBufferPtr - (uint8_t *)m_triVertexBufferBase;
    m_triVertexBuffer->setData((void *)m_triVertexBufferBase, triDataSize);

    m_triShader->bind();
    const uint32_t triCount =
        m_triIndexCount ? m_triIndexCount
                        : m_triVertexArray->getIndexBuffer()->getCount();
    glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  if (m_quadIndexCount) {
    m_quadVertexArray->bind();
    const uint32_t quadDataSize =
        (uint8_t *)m_quadVertexBufferPtr - (uint8_t *)m_quadVertexBufferBase;
    m_quadVertexBuffer->setData((void *)m_quadVertexBufferBase, quadDataSize);

    // bind textures
    for (uint32_t i = 0; i < m_textureSlotIndex; i++)
      m_textureSlots[i]->bind(i);

    m_quadTextureShader->bind();
    const uint32_t quadCount =
        m_quadIndexCount ? m_quadIndexCount
                         : m_quadVertexArray->getIndexBuffer()->getCount();
    glDrawElements(GL_TRIANGLES, quadCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}
void VertexBatch::bindTextures()
{
  for (uint32_t i = 0; i < m_textureSlotIndex; i++)
    m_textureSlots[i]->bind(i);
}

VertexBatch::VertexBatch()
{

  // ====== Quads ================================================== //
  m_quadVertexArray.reset(new VertexArray());

  m_quadVertexBuffer.reset(
      new VertexBuffer(MaxQuadVertices * sizeof(QuadVertex)));
  m_quadVertexBuffer->setLayout({{ShaderDataType::Float3, "a_Position"}, //
                                 {ShaderDataType::Float4, "a_Color"},    //
                                 {ShaderDataType::Float2, "a_TexCoord"}, //
                                 {ShaderDataType::Float, "a_TexIndex"},  //
                                 {ShaderDataType::Float, "a_TilingFactor"}});
  m_quadVertexArray->addVertexBuffer(m_quadVertexBuffer);
  m_quadVertexBufferBase = new QuadVertex[MaxQuadVertices];

  // indices
  uint32_t *quadIndices = new uint32_t[MaxQuadIndices];
  for (uint32_t i = 0, offset = 0; i < MaxQuadIndices; i += 6, offset += 4) {
    quadIndices[i + 0] = offset + 0;
    quadIndices[i + 1] = offset + 1;
    quadIndices[i + 2] = offset + 2;

    quadIndices[i + 3] = offset + 2;
    quadIndices[i + 4] = offset + 3;
    quadIndices[i + 5] = offset + 0;
  }
  std::shared_ptr<IndexBuffer> quadIB;
  quadIB.reset(new IndexBuffer(quadIndices, MaxQuadIndices));
  m_quadVertexArray->setIndexBuffer(quadIB);
  delete[] quadIndices;

  m_whiteTexture.reset(new Texture(1, 1));
  const uint32_t whiteTextureData = 0xffffffff;
  m_whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

  int32_t samplers[MaxTextureSlots];
  for (uint32_t i = 0; i < MaxTextureSlots; i++) {
    samplers[i] = i;
  }

  m_quadTextureShader.reset(new Shader("resources/shaders/Texture.glsl"));
  m_quadTextureShader->bind();
  m_quadTextureShader->uploadUniformIntArray("u_Textures", samplers,
                                             MaxTextureSlots);
  m_textureSlots[0] = m_whiteTexture;

  // ====== Triangles ============================================== //
  m_triVertexArray.reset(new VertexArray());

  m_triVertexBuffer.reset(new VertexBuffer(MaxTriVertices * sizeof(TriVertex)));
  m_triVertexBuffer->setLayout({{ShaderDataType::Float3, "a_Position"},
                                {ShaderDataType::Float4, "a_Color"}});
  m_triVertexArray->addVertexBuffer(m_triVertexBuffer);
  m_triVertexBufferBase = new TriVertex[MaxTriVertices];

  // indices
  uint32_t *triIndices = new uint32_t[MaxTriIndices];
  for (uint32_t i = 0, offset = 0; i < MaxTriIndices; i += 3, offset += 3) {
    triIndices[i + 0] = offset + 0;
    triIndices[i + 1] = offset + 1;
    triIndices[i + 2] = offset + 2;
  }
  std::shared_ptr<IndexBuffer> triIB;
  triIB.reset(new IndexBuffer(triIndices, MaxTriIndices));
  m_triVertexArray->setIndexBuffer(triIB);
  delete[] triIndices;

  m_triShader.reset(new Shader("resources/shaders/Triangles.glsl"));
}

// ================================================================= //
// Draws
// ================================================================= //

void VertexBatch::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                           const glm::vec4 &tintColor, const float textureIndex,
                           const float tilingFactor,
                           const std::array<glm::vec2, 4> &textureCoordinate)
{
  const glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), {position, 0.f}) *
      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  for (int i = 0; i < 4; i++) {
    m_quadVertexBufferPtr->position = transform * m_quadVertexPositions[i];
    m_quadVertexBufferPtr->color = tintColor;
    m_quadVertexBufferPtr->texCoord = textureCoordinate[i];
    m_quadVertexBufferPtr->texIndex = textureIndex;
    m_quadVertexBufferPtr->tilingFactor = tilingFactor;
    m_quadVertexBufferPtr++;
  }
  m_quadIndexCount += 6;
}

void VertexBatch::drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                           const glm::vec4 &tintColor,
                           const std::shared_ptr<Texture> &texture,
                           const float tilingFactor,
                           const std::array<glm::vec2, 4> &textureCoordinate)
{
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
  drawQuad(position, size, tintColor, textureIndex, tilingFactor,
           textureCoordinate);
}

// ================================================================= //
// Draws with rotation
// ================================================================= //

void VertexBatch::drawRotQuad(const glm::vec2 &position, const glm::vec2 &size,
                              const glm::vec4 &tintColor,
                              const float textureIndex,
                              const float tilingFactor,
                              const std::array<glm::vec2, 4> &textureCoordinate,
                              const float rotationAngleRadians)
{
  const glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), {position, 0.f}) *
      glm::rotate(glm::mat4(1.0f), rotationAngleRadians, {0.0f, 0.0f, 1.0f}) *
      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  for (int i = 0; i < 4; i++) {
    m_quadVertexBufferPtr->position = transform * m_quadVertexPositions[i];
    m_quadVertexBufferPtr->color = tintColor;
    m_quadVertexBufferPtr->texCoord = textureCoordinate[i];
    m_quadVertexBufferPtr->texIndex = textureIndex;
    m_quadVertexBufferPtr->tilingFactor = tilingFactor;
    m_quadVertexBufferPtr++;
  }
  m_quadIndexCount += 6;
}

void VertexBatch::drawRotQuad(const glm::vec2 &position, const glm::vec2 &size,
                              const glm::vec4 &tintColor,
                              const std::shared_ptr<Texture> &texture,
                              const float tilingFactor,
                              const std::array<glm::vec2, 4> &textureCoordinate,
                              const float rotationAngleRadians)
{
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
  drawRotQuad(position, size, tintColor, textureIndex, tilingFactor,
              textureCoordinate, rotationAngleRadians);
}

// ================================================================= //
// Draws with Triangles
// ================================================================= //
void VertexBatch::drawTri(const glm::vec2 &position, const glm::vec2 &size,
                          const glm::vec4 &tintColor)
{
  const glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), {position, 0.f}) *
      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  for (int i = 0; i < 4; i++) {
    m_triVertexBufferPtr->position = transform * m_triVertexPositions[i];
    m_triVertexBufferPtr->color = tintColor;
    m_triVertexBufferPtr++;
  }
  m_triIndexCount += 3;
}

void VertexBatch::drawRotTri(const glm::vec2 &position, const glm::vec2 &size,
                             const glm::vec4 &tintColor,
                             const float rotationAngleRadians)
{
  const glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), {position, 0.f}) *
      glm::rotate(glm::mat4(1.0f), rotationAngleRadians, {0.0f, 0.0f, 1.0f}) *
      glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

  for (int i = 0; i < 4; i++) {
    m_triVertexBufferPtr->position = transform * m_triVertexPositions[i];
    m_triVertexBufferPtr->color = tintColor;
    m_triVertexBufferPtr++;
  }
  m_triIndexCount += 3;
}
} // namespace pain
