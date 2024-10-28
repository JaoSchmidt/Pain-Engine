#include "CoreRender/Renderer/Renderer2d.h"

#include "CoreRender/ShaderManager.h"
#include "ECS/Components/Particle.h"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace pain
{
// quads initializer
std::shared_ptr<VertexArray> Renderer2d::m_quadVertexArray = nullptr;
std::shared_ptr<VertexBuffer> Renderer2d::m_quadVertexBuffer = nullptr;
std::shared_ptr<Shader> Renderer2d::m_quadTextureShader = nullptr;
QuadVertex *Renderer2d::m_quadVertexBufferBase = nullptr;
QuadVertex *Renderer2d::m_quadVertexBufferPtr = nullptr;
uint32_t Renderer2d::m_quadIndexCount = 0; // at init, there are 0 quads

// tri initializer
std::shared_ptr<VertexArray> Renderer2d::m_triVertexArray = nullptr;
std::shared_ptr<VertexBuffer> Renderer2d::m_triVertexBuffer = nullptr;
std::shared_ptr<Shader> Renderer2d::m_triShader = nullptr;
TriVertex *Renderer2d::m_triVertexBufferBase = nullptr;
TriVertex *Renderer2d::m_triVertexBufferPtr = nullptr;
uint32_t Renderer2d::m_triIndexCount = 0; // at init, there are 0 tirangles

// spray particle initializer
std::shared_ptr<VertexArray> Renderer2d::m_sprayVertexArray = nullptr;
std::shared_ptr<VertexBuffer> Renderer2d::m_sprayVertexBuffer = nullptr;
std::shared_ptr<Shader> Renderer2d::m_sprayShader = nullptr;
ParticleVertex *Renderer2d::m_sprayVertexBufferBase = nullptr;
ParticleVertex *Renderer2d::m_sprayVertexBufferPtr = nullptr;
uint32_t Renderer2d::m_sprayIndexCount = 0;

// texture initializer
std::shared_ptr<Texture> Renderer2d::m_whiteTexture = nullptr;
std::array<std::shared_ptr<Texture>, Renderer2d::MaxTextureSlots>
    Renderer2d::m_textureSlots = {nullptr};
uint32_t Renderer2d::m_textureSlotIndex =
    1; // at init, there is 1 white texture

void Renderer2d::initBatches()
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

  m_quadTextureShader.reset(new Shader("resources/shaders/Texture.glsl"));
  m_quadTextureShader->bind();
  m_quadTextureShader->uploadUniformIntArray("u_Textures", samplers,
                                             MaxTextureSlots);
  m_textureSlots[0] = m_whiteTexture;

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

  m_triShader.reset(new Shader("resources/shaders/Triangles.glsl"));

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

  m_sprayShader.reset(new Shader("resources/shaders/SprayParticles.glsl"));
  ShaderLibrary::getInstance()->add(m_sprayShader);

  // =============================================================== //
  // Text Rendering
  // =============================================================== //
  // only one font for now
  // m_textureFont
  //     .reset(ftgl::texture_font_new_from_file(m_atlas, const float pt_size,
  //                                             const char *filename))
  m_textVertexArray.reset(new VertexArray());

  m_textVertexBuffer.reset(
      new VertexBuffer(MaxTextVertices * sizeof(TextVertex)));
  m_textVertexBuffer->setLayout({{ShaderDataType::Float3, "a_position"}, //
                                 {ShaderDataType::Float4, "a_color"},
                                 {ShaderDataType::Float2, "a_texCoord"},
                                 {ShaderDataType::Float, "a_shift"},
                                 {ShaderDataType::Float, "a_gamma"}});
  m_textVertexArray->addVertexBuffer(m_textVertexBuffer);
  m_textVertexBufferBase = new TextVertex[MaxTextVertices];

  // indices
  uint32_t *textIndices = new uint32_t[MaxTextIndices];
  for (uint32_t i = 0, offset = 0; i < MaxTextIndices; i += 6, offset += 4) {
    textIndices[i + 0] = offset + 0;
    textIndices[i + 1] = offset + 1;
    textIndices[i + 2] = offset + 2;

    textIndices[i + 3] = offset + 0;
    textIndices[i + 4] = offset + 2;
    textIndices[i + 5] = offset + 3;
  }
  std::shared_ptr<IndexBuffer> textIB;
  textIB.reset(new IndexBuffer(textIndices, MaxTextIndices));
  m_textVertexArray->setIndexBuffer(textIB);
  delete[] textIndices;

  m_textShader.reset(new Shader("resources/shaders/Text.glsl"));
  m_textureAtlas.reset(
      ftgl::texture_atlas_new(512, 512, 3)); // 3 for lcd on, 1 for lcd off
  // m_whiteTexture.reset(new Texture(1, 1));
  // const uint32_t whiteTextureData = 0xffffffff;
  // m_whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

  // int32_t samplers[MaxTextureSlots];
  // for (uint32_t i = 0; i < MaxTextureSlots; i++) {
  //   samplers[i] = i;
  // }

  // m_quadTextureShader.reset(new Shader("resources/shaders/Texture.glsl"));
  // m_quadTextureShader->bind();
  // m_quadTextureShader->uploadUniformIntArray("u_Textures", samplers,
  //                                            MaxTextureSlots);
  // m_textureSlots[0] = m_whiteTexture;
}

void Renderer2d::drawBatches(const glm::mat4 &viewProjectionMatrix)
{
  // =============================================================== //
  // Quads
  // =============================================================== //
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
  // =============================================================== //
  // Spray Particles
  // =============================================================== //
  if (m_sprayIndexCount) {
    m_sprayVertexArray->bind();
    const uint32_t sprayDataSize =
        (uint8_t *)m_sprayVertexBufferPtr - (uint8_t *)m_sprayVertexBufferBase;
    m_sprayVertexBuffer->setData((void *)m_sprayVertexBufferBase,
                                 sprayDataSize);

    // bind textures
    for (uint32_t i = 0; i < m_textureSlotIndex; i++)
      m_textureSlots[i]->bind(i);

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

void Renderer2d::bindTextures()
{
  for (uint32_t i = 0; i < m_textureSlotIndex; i++)
    m_textureSlots[i]->bind(i);
}

void Renderer2d::goBackToFirstVertex()
{
  m_quadIndexCount = 0;
  m_quadVertexBufferPtr = m_quadVertexBufferBase;

  m_triIndexCount = 0;
  m_triVertexBufferPtr = m_triVertexBufferBase;

  m_sprayIndexCount = 0;
  m_sprayVertexBufferPtr = m_sprayVertexBufferBase;
}

float Renderer2d::allocateTextures(const std::shared_ptr<Texture> &texture)
{
  // NOTE: this can be optimized later to avoid searching the texture
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
  return textureIndex;
}
const glm::mat4 Renderer2d::getTransform(const glm::vec2 &position,
                                         const glm::vec2 &size)
{
  return glm::translate(glm::mat4(1.0f), {position, 0.f}) *
         glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
}
const glm::mat4 Renderer2d::getTransform(const glm::vec2 &position,
                                         const glm::vec2 &size,
                                         const float rotationAngleRadians)
{
  return glm::translate(glm::mat4(1.0f), {position, 0.f}) *
         glm::rotate(glm::mat4(1.0f), rotationAngleRadians,
                     {0.0f, 0.0f, 1.0f}) *
         glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
}

void Renderer2d::allocateQuad(const glm::mat4 &transform,
                              const glm::vec4 &tintColor,
                              const float tilingFactor,
                              const float textureIndex,
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

void Renderer2d::allocateTri(const glm::mat4 &transform,
                             const glm::vec4 &tintColor)
{
  for (int i = 0; i < 3; i++) {
    m_triVertexBufferPtr->position = transform * m_triVertexPositions[i];
    m_triVertexBufferPtr->color = tintColor;
    m_triVertexBufferPtr++;
  }
  m_triIndexCount += 3;
}

void Renderer2d::beginSprayParticle(const float globalTime,
                                    const ParticleSprayComponent &psc)
{
  m_sprayShader->bind();
  m_sprayShader->uploadUniformFloat("u_Time", globalTime);
  m_sprayShader->uploadUniformFloat("u_ParticleVelocity", psc.m_velocity);
  m_sprayShader->uploadUniformFloat("u_LifeTime", psc.m_lifeTime);
  m_sprayShader->uploadUniformFloat("u_SizeChangeSpeed", psc.m_sizeChangeSpeed);
  m_sprayShader->uploadUniformFloat("u_randomSizeFactor", psc.m_randSizeFactor);
}
void Renderer2d::allocateSprayParticles(const glm::vec2 &position,
                                        const glm::vec2 &offset,
                                        const glm::vec2 &normal,
                                        const float startTime,
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

} // namespace pain
