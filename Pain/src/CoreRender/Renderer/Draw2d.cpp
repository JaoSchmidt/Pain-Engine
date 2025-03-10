#include "CoreRender/Renderer/Renderer2d.h"

#include "ECS/Components/Particle.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace pain
{
namespace
{
constexpr glm::vec4 m_quadVertexPositions[4] = {
    glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
    glm::vec4(0.5f, -0.5f, 0.f, 1.f),
    glm::vec4(0.5f, 0.5f, 0.f, 1.f),
    glm::vec4(-0.5f, 0.5f, 0.f, 1.f),
};
constexpr glm::vec4 m_triVertexPositions[3] = {
    glm::vec4(0.0f, 0.5f, 0.f, 1.f),
    glm::vec4(0.5f, -0.5f, 0.f, 1.f),
    glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
};
constexpr glm::vec2 m_sprayVertexPositions[4] = {
    glm::vec2(-0.5f, -0.5f),
    glm::vec2(0.5f, -0.5f),
    glm::vec2(0.5f, 0.5f),
    glm::vec2(-0.5f, 0.5f),
};
constexpr uint32_t MaxQuads = 40000;
constexpr uint32_t MaxQuadVertices = MaxQuads * 4;
constexpr uint32_t MaxQuadIndices = MaxQuads * 6;
constexpr uint32_t MaxTri = 2000;
constexpr uint32_t MaxTriVertices = MaxTri * 3;
constexpr uint32_t MaxTriIndices = MaxTri * 3;
constexpr uint32_t MaxSprayParticles = 10000;
constexpr uint32_t MaxSprayVertices = MaxSprayParticles * 4;
constexpr uint32_t MaxSprayIndices = MaxSprayParticles * 6;
// quads initializer
std::shared_ptr<VertexArray> m_quadVertexArray = nullptr;
std::shared_ptr<VertexBuffer> m_quadVertexBuffer = nullptr;
std::shared_ptr<Shader> m_quadTextureShader = nullptr;
QuadVertex *m_quadVertexBufferBase = nullptr;
QuadVertex *m_quadVertexBufferPtr = nullptr;
uint32_t m_quadIndexCount = 0; // at init, there are 0 quads

// text vertex initializer
std::shared_ptr<VertexArray> m_textVertexArray = nullptr;
std::shared_ptr<VertexBuffer> m_textVertexBuffer = nullptr;
std::shared_ptr<Shader> m_textTextureShader = nullptr;
TextQuadVertex *m_textVertexBufferBase = nullptr;
TextQuadVertex *m_textVertexBufferPtr = nullptr;
uint32_t m_textIndexCount = 0; // at init, there are 0 texts

// tri initializer
std::shared_ptr<VertexArray> m_triVertexArray = nullptr;
std::shared_ptr<VertexBuffer> m_triVertexBuffer = nullptr;
std::shared_ptr<Shader> m_triShader = nullptr;
TriVertex *m_triVertexBufferBase = nullptr;
TriVertex *m_triVertexBufferPtr = nullptr;
uint32_t m_triIndexCount = 0; // at init, there are 0 tirangles

// spray particle initializer
std::shared_ptr<VertexArray> m_sprayVertexArray = nullptr;
std::shared_ptr<VertexBuffer> m_sprayVertexBuffer = nullptr;
std::shared_ptr<Shader> m_sprayShader = nullptr;
ParticleVertex *m_sprayVertexBufferBase = nullptr;
ParticleVertex *m_sprayVertexBufferPtr = nullptr;
uint32_t m_sprayIndexCount = 0;

// texture initializer
constexpr uint32_t MaxTextureSlots = 32;
std::shared_ptr<Texture> m_whiteTexture = nullptr;
std::array<Texture *, MaxTextureSlots> m_textureSlots = {nullptr};
uint32_t m_textureSlotIndex = 1; // at init, there is 1 white texture
} // namespace

namespace Renderer2d
{
const Texture *m_fontAtlasTexture = nullptr;

void uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                         float globalTime, const glm::mat4 &transform)
{
  m_quadTextureShader->bind();
  m_quadTextureShader->uploadUniformMat4("u_ViewProjection",
                                         viewProjectionMatrix);
  m_quadTextureShader->uploadUniformMat4("u_Transform", transform);

  m_triShader->bind();
  m_triShader->uploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
  m_triShader->uploadUniformMat4("u_Transform", transform);

  m_sprayShader->bind();
  m_sprayShader->uploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
  m_sprayShader->uploadUniformMat4("u_Transform", transform);
  m_sprayShader->uploadUniformFloat("u_Time", globalTime);

  m_textTextureShader->bind();
  m_textTextureShader->uploadUniformMat4("u_ViewProjection",
                                         viewProjectionMatrix);
  m_textTextureShader->uploadUniformMat4("u_Transform", transform);
}
void initBatches()
{
  // =============================================================== //
  // Quads
  // =============================================================== //
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

  m_quadTextureShader.reset(
      new Shader("resources/default/shaders/Texture.glsl"));
  m_quadTextureShader->bind();
  m_quadTextureShader->uploadUniformIntArray("u_Textures", samplers,
                                             MaxTextureSlots);
  m_textureSlots[0] = m_whiteTexture.get();

  // =============================================================== //
  // Triangles
  // =============================================================== //
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

  m_triShader.reset(new Shader("resources/default/shaders/Triangles.glsl"));
  // =============================================================== //
  // Text
  // =============================================================== //
  m_textVertexArray.reset(new VertexArray());

  m_textVertexBuffer.reset(
      new VertexBuffer(MaxQuadVertices * sizeof(TextQuadVertex)));
  m_textVertexBuffer->setLayout({
      {ShaderDataType::Float3, "a_Position"}, //
      {ShaderDataType::Float4, "a_Color"},    //
      {ShaderDataType::Float2, "a_TexCoord"}, //
  });
  m_textVertexArray->addVertexBuffer(m_textVertexBuffer);
  m_textVertexBufferBase = new TextQuadVertex[MaxQuadVertices];
  m_textVertexArray->setIndexBuffer(quadIB);

  m_textTextureShader.reset(
      new Shader("resources/default/shaders/Renderer2dText.glsl"));
  m_textTextureShader->bind();
  // =============================================================== //
  // Spray Particles
  // =============================================================== //
  m_sprayVertexArray.reset(new VertexArray());

  m_sprayVertexBuffer.reset(
      new VertexBuffer(MaxSprayVertices * sizeof(ParticleVertex)));
  m_sprayVertexBuffer->setLayout({{ShaderDataType::Float2, "a_Position"}, //
                                  {ShaderDataType::Float2, "a_Offset"},
                                  {ShaderDataType::Float2, "a_Normal"},
                                  {ShaderDataType::Float, "a_Time"},
                                  {ShaderDataType::Float, "a_RotationSpeed"}});
  m_sprayVertexArray->addVertexBuffer(m_sprayVertexBuffer);
  m_sprayVertexBufferBase = new ParticleVertex[MaxSprayVertices];

  // indices
  uint32_t *sprayIndices = new uint32_t[MaxSprayIndices];
  for (uint32_t i = 0, offset = 0; i < MaxSprayIndices; i += 6, offset += 4) {
    sprayIndices[i + 0] = offset + 0;
    sprayIndices[i + 1] = offset + 1;
    sprayIndices[i + 2] = offset + 2;

    sprayIndices[i + 3] = offset + 2;
    sprayIndices[i + 4] = offset + 3;
    sprayIndices[i + 5] = offset + 0;
  }
  std::shared_ptr<IndexBuffer> sprayIB;
  sprayIB.reset(new IndexBuffer(sprayIndices, MaxSprayIndices));
  m_sprayVertexArray->setIndexBuffer(sprayIB);
  delete[] sprayIndices;

  m_sprayShader.reset(
      new Shader("resources/default/shaders/SprayParticles.glsl"));
}

void drawBatches(const glm::mat4 &viewProjectionMatrix)
{
  // =============================================================== //
  // Quads
  // =============================================================== //
  // PLOG_I("bind texture id = {}", m_textureSlots[i]->getRendererId());

  if (m_quadIndexCount) {
    m_quadVertexArray->bind();
    const uint32_t quadDataSize =
        (uint8_t *)m_quadVertexBufferPtr - (uint8_t *)m_quadVertexBufferBase;
    m_quadVertexBuffer->setData((void *)m_quadVertexBufferBase, quadDataSize);

    // bind textures
    for (uint32_t i = 0; i < m_textureSlotIndex; i++) {
      m_textureSlots[i]->bindToSlot(i);
    }

    m_quadTextureShader->bind();
    const uint32_t quadCount =
        m_quadIndexCount ? m_quadIndexCount
                         : m_quadVertexArray->getIndexBuffer()->getCount();
    glDrawElements(GL_TRIANGLES, quadCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  // =============================================================== //
  // Text
  // =============================================================== //
  if (m_textIndexCount) {
    m_textVertexArray->bind();
    const uint32_t textDataSize =
        (uint8_t *)m_textVertexBufferPtr - (uint8_t *)m_textVertexBufferBase;
    m_textVertexBuffer->setData((void *)m_textVertexBufferBase, textDataSize);

    m_fontAtlasTexture->bind();
    m_textTextureShader->bind();
    m_textTextureShader->uploadUniformInt("u_FontAtlas", 0);

    const uint32_t textCount =
        m_textIndexCount ? m_textIndexCount
                         : m_textVertexArray->getIndexBuffer()->getCount();
    glDrawElements(GL_TRIANGLES, textCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // =============================================================== //
  // Spray Particles
  // =============================================================== //
  if (m_sprayIndexCount) {
    m_sprayVertexArray->bind();
    const uint32_t sprayDataSize =
        (uint8_t *)m_sprayVertexBufferPtr - (uint8_t *)m_sprayVertexBufferBase;
    m_sprayVertexBuffer->setData((void *)m_sprayVertexBufferBase,
                                 sprayDataSize);

    // FIX: I don't really need to bind textures here, right?
    // TODO: make it possible to bind textures for spray particles
    for (uint32_t i = 0; i < m_textureSlotIndex; i++)
      m_textureSlots[i]->bindAndClearSlot();

    m_sprayShader->bind();
    const uint32_t sprayCount =
        m_sprayIndexCount ? m_sprayIndexCount
                          : m_sprayVertexArray->getIndexBuffer()->getCount();
    glDrawElements(GL_TRIANGLES, sprayCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  // =============================================================== //
  // Triangles
  // =============================================================== //
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
}

void bindTextures()
{
  for (uint32_t i = 0; i < m_textureSlotIndex; i++)
    m_textureSlots[i]->bindToSlot(i);
}

void goBackToFirstVertex()
{
  m_quadIndexCount = 0;
  m_quadVertexBufferPtr = m_quadVertexBufferBase;

  m_triIndexCount = 0;
  m_triVertexBufferPtr = m_triVertexBufferBase;

  m_sprayIndexCount = 0;
  m_sprayVertexBufferPtr = m_sprayVertexBufferBase;

  m_textIndexCount = 0;
  m_textVertexBufferPtr = m_textVertexBufferBase;

  m_textureSlotIndex = 1; // 1, because 0 is for default 1x1 white texture
}

// TODO: allow the possibility of removing the texture (tho you could just reset
// everything after first delete)
float allocateTextures(Texture &texture)
{
  // NOTE: this can be optimized later to avoid searching the texture
  float textureIndex = 0.0f;
  // use it to allocate new texture
  if (texture.m_slot == 0) {
    textureIndex = (float)m_textureSlotIndex;
    m_textureSlots[m_textureSlotIndex] = &texture;
    texture.m_slot = m_textureSlotIndex;
    m_textureSlotIndex++;
  } else {
    textureIndex = (float)texture.m_slot;
  }
  // TODO: check if m_textureSlotIndex is bigger than 32, then flush

  P_ASSERT_W(textureIndex != 0.0f,
             "Missing texture inside a drawQuad that requires textures");
  return textureIndex;
}
void removeTexture(const Texture &texture)
{
  if (texture.m_slot == 0) // m_textureSlots doesn't have the texture
    return;
  P_ASSERT_W(*m_textureSlots[texture.m_slot] == texture,
             "Attempted to remove a texture that wasn't allocated.");
  for (uint32_t i = texture.m_slot; i < m_textureSlotIndex; i++) {
    m_textureSlots[i] = m_textureSlots[i + 1];
    m_textureSlots[i]->m_slot = i; // Update the slot value in the Texture
  }

  m_textureSlots[m_textureSlotIndex] = nullptr; // Clear last slot
  m_textureSlotIndex--;
  return;
}
const glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size)
{
  return glm::translate(glm::mat4(1.0f), {position, 0.f}) *
         glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
}
const glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size,
                             const float rotationAngleRadians)
{
  return glm::translate(glm::mat4(1.0f), {position, 0.f}) *
         glm::rotate(glm::mat4(1.0f), rotationAngleRadians,
                     {0.0f, 0.0f, 1.0f}) *
         glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
}

void allocateQuad(const glm::mat4 &transform, const glm::vec4 &tintColor,
                  const float tilingFactor, const float textureIndex,
                  const std::array<glm::vec2, 4> &textureCoordinate)
{
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

void allocateTri(const glm::mat4 &transform, const glm::vec4 &tintColor)
{
  for (int i = 0; i < 3; i++) {
    m_triVertexBufferPtr->position = transform * m_triVertexPositions[i];
    m_triVertexBufferPtr->color = tintColor;
    m_triVertexBufferPtr++;
  }
  m_triIndexCount += 3;
}

void beginSprayParticle(const float globalTime,
                        const ParticleSprayComponent &psc)
{
  m_sprayShader->bind();
  m_sprayShader->uploadUniformFloat("u_Time", globalTime);
  m_sprayShader->uploadUniformFloat("u_ParticleVelocity", psc.m_velocity);
  m_sprayShader->uploadUniformFloat("u_LifeTime", psc.m_lifeTime);
  m_sprayShader->uploadUniformFloat("u_SizeChangeSpeed", psc.m_sizeChangeSpeed);
  m_sprayShader->uploadUniformFloat("u_randomSizeFactor", psc.m_randSizeFactor);
}
void allocateSprayParticles(const glm::vec2 &position, const glm::vec2 &offset,
                            const glm::vec2 &normal, const float startTime,
                            const float rotationSpeed)
{
  for (int i = 0; i < 4; i++) {
    m_sprayVertexBufferPtr->position = m_sprayVertexPositions[i];
    m_sprayVertexBufferPtr->offset = offset;
    m_sprayVertexBufferPtr->normal = normal;
    m_sprayVertexBufferPtr->time = startTime;
    m_sprayVertexBufferPtr->rotationSpeed = rotationSpeed;
    m_sprayVertexBufferPtr++;
  }
  m_sprayIndexCount += 6;
}

void allocateCharacter(const glm::mat4 &transform, const glm::vec4 &tintColor,
                       const std::array<glm::vec2, 4> &textureCoordinate,
                       const std::array<glm::vec4, 4> &textVertexPositions)
{
  for (int i = 0; i < 4; i++) {
    m_textVertexBufferPtr->position = transform * textVertexPositions[i];
    m_textVertexBufferPtr->color = tintColor;
    m_textVertexBufferPtr->texCoord = textureCoordinate[i];
    m_textVertexBufferPtr++;
  }
  m_textIndexCount += 6;
}
} // namespace Renderer2d

} // namespace pain
