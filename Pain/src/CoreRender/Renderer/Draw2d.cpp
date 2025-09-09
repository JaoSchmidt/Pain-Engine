#include "CoreRender/Renderer/Renderer2d.h"

#include "Assets/ResourceManager.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/Particle.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace pain
{
namespace
{
constexpr glm::vec4 QuadVertexPositions[4] = {
    glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
    glm::vec4(0.5f, -0.5f, 0.f, 1.f),
    glm::vec4(0.5f, 0.5f, 0.f, 1.f),
    glm::vec4(-0.5f, 0.5f, 0.f, 1.f),
};
constexpr glm::vec4 TriVertexPositions[3] = {
    glm::vec4(0.0f, 0.5f, 0.f, 1.f),
    glm::vec4(0.5f, -0.5f, 0.f, 1.f),
    glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
};
constexpr glm::vec2 SprayVertexPositions[4] = {
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
} // namespace

const Texture *m_fontAtlasTexture = nullptr;

std::optional<Renderer2d>
Renderer2d::createRenderer2d(const OrthoCamera &cameratEntity)
{

  PROFILE_FUNCTION();
  // NOTE: This enable 3d and can be changed later in case we need some camera
  // mechanic
  // glEnable(GL_DEPTH_TEST);
  PROFILE_FUNCTION();
  // =============================================================== //
  // Quads
  // =============================================================== //
  VertexArray quadVertexArray = {};

  VertexBuffer quadVertexBuffer =
      VertexBuffer(MaxQuadVertices * sizeof(QuadVertex));
  quadVertexBuffer->setLayout({{ShaderDataType::Float3, "a_Position"}, //
                               {ShaderDataType::Float4, "a_Color"},    //
                               {ShaderDataType::Float2, "a_TexCoord"}, //
                               {ShaderDataType::Float, "a_TexIndex"},  //
                               {ShaderDataType::Float, "a_TilingFactor"}});
  quadVertexArray->addVertexBuffer(quadVertexBuffer);
  quadVertexBufferBase = new QuadVertex[MaxQuadVertices];

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
  quadIB = new IndexBuffer(quadIndices, MaxQuadIndices);
  quadVertexArray->setIndexBuffer(quadIB);
  delete[] quadIndices;

  int32_t samplers[MaxTextureSlots];
  for (uint32_t i = 0; i < MaxTextureSlots; i++) {
    samplers[i] = i;
  }

  quadTextureShader = new Shader("resources/default/shaders/Texture.glsl");
  quadTextureShader->bind();
  quadTextureShader->uploadUniformIntArray("u_Textures", samplers,
                                           MaxTextureSlots);
  whiteTexture = &resources::getDefaultTexture(resources::BLANK);
  textureSlots[0] = whiteTexture;

  // =============================================================== //
  // Triangles
  // =============================================================== //
  triVertexArray = new VertexArray();

  triVertexBuffer = new VertexBuffer(MaxTriVertices * sizeof(TriVertex));
  triVertexBuffer->setLayout({{ShaderDataType::Float3, "a_Position"},
                              {ShaderDataType::Float4, "a_Color"}});
  triVertexArray->addVertexBuffer(triVertexBuffer);
  triVertexBufferBase = new TriVertex[MaxTriVertices];

  // indices
  uint32_t *triIndices = new uint32_t[MaxTriIndices];
  for (uint32_t i = 0, offset = 0; i < MaxTriIndices; i += 3, offset += 3) {
    triIndices[i + 0] = offset + 0;
    triIndices[i + 1] = offset + 1;
    triIndices[i + 2] = offset + 2;
  }
  triIB = new IndexBuffer(triIndices, MaxTriIndices);
  triVertexArray->setIndexBuffer(triIB);
  delete[] triIndices;

  triShader = new Shader("resources/default/shaders/Triangles.glsl");
  // =============================================================== //
  // Text
  // =============================================================== //
  textVertexArray = new VertexArray();

  textVertexBuffer = new VertexBuffer(MaxQuadVertices * sizeof(TextQuadVertex));
  textVertexBuffer->setLayout({
      {ShaderDataType::Float3, "a_Position"}, //
      {ShaderDataType::Float4, "a_Color"},    //
      {ShaderDataType::Float2, "a_TexCoord"}, //
  });
  textVertexArray->addVertexBuffer(textVertexBuffer);
  textVertexBufferBase = new TextQuadVertex[MaxQuadVertices];
  textVertexArray->setIndexBuffer(quadIB);

  textTextureShader =
      new Shader("resources/default/shaders/Renderer2dText.glsl");
  textTextureShader->bind();
  // =============================================================== //
  // Spray Particles
  // =============================================================== //
  sprayVertexArray = new VertexArray();

  sprayVertexBuffer =
      new VertexBuffer(MaxSprayVertices * sizeof(ParticleVertex));
  sprayVertexBuffer->setLayout({{ShaderDataType::Float2, "a_Position"}, //
                                {ShaderDataType::Float2, "a_Offset"},
                                {ShaderDataType::Float2, "a_Normal"},
                                {ShaderDataType::Float, "a_Time"},
                                {ShaderDataType::Float, "a_RotationSpeed"}});
  sprayVertexArray->addVertexBuffer(sprayVertexBuffer);
  sprayVertexBufferBase = new ParticleVertex[MaxSprayVertices];

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
  sprayIB = new IndexBuffer(sprayIndices, MaxSprayIndices);
  sprayVertexArray->setIndexBuffer(sprayIB);
  delete[] sprayIndices;

  sprayShader = new Shader("resources/default/shaders/SprayParticles.glsl");

  // allow transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // HACK: allow textures with 3 channels to align properly, e.g. font textures.
  // No idea why it works tho, perhaps I will find a proper doc later
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  m_cameraEntity = &cameraEntity;
  return Renderer2d();
}

void Renderer2d::initBatches()
{
  PROFILE_FUNCTION();
  // =============================================================== //
  // Quads
  // =============================================================== //
  m_quadVertexArray = new VertexArray();

  m_quadVertexBuffer = new VertexBuffer(MaxQuadVertices * sizeof(QuadVertex));
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
  m_quadIB = new IndexBuffer(quadIndices, MaxQuadIndices);
  m_quadVertexArray->setIndexBuffer(m_quadIB);
  delete[] quadIndices;

  int32_t samplers[MaxTextureSlots];
  for (uint32_t i = 0; i < MaxTextureSlots; i++) {
    samplers[i] = i;
  }

  m_quadTextureShader = new Shader("resources/default/shaders/Texture.glsl");
  m_quadTextureShader->bind();
  m_quadTextureShader->uploadUniformIntArray("u_Textures", samplers,
                                             MaxTextureSlots);
  m_whiteTexture = &resources::getDefaultTexture(resources::BLANK);
  m_textureSlots[0] = m_whiteTexture;

  // =============================================================== //
  // Triangles
  // =============================================================== //
  m_triVertexArray = new VertexArray();

  m_triVertexBuffer = new VertexBuffer(MaxTriVertices * sizeof(TriVertex));
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
  m_triIB = new IndexBuffer(triIndices, MaxTriIndices);
  m_triVertexArray->setIndexBuffer(m_triIB);
  delete[] triIndices;

  m_triShader = new Shader("resources/default/shaders/Triangles.glsl");
  // =============================================================== //
  // Text
  // =============================================================== //
  m_textVertexArray = new VertexArray();

  m_textVertexBuffer =
      new VertexBuffer(MaxQuadVertices * sizeof(TextQuadVertex));
  m_textVertexBuffer->setLayout({
      {ShaderDataType::Float3, "a_Position"}, //
      {ShaderDataType::Float4, "a_Color"},    //
      {ShaderDataType::Float2, "a_TexCoord"}, //
  });
  m_textVertexArray->addVertexBuffer(m_textVertexBuffer);
  m_textVertexBufferBase = new TextQuadVertex[MaxQuadVertices];
  m_textVertexArray->setIndexBuffer(m_quadIB);

  m_textTextureShader =
      new Shader("resources/default/shaders/Renderer2dText.glsl");
  m_textTextureShader->bind();
  // =============================================================== //
  // Spray Particles
  // =============================================================== //
  m_sprayVertexArray = new VertexArray();

  m_sprayVertexBuffer =
      new VertexBuffer(MaxSprayVertices * sizeof(ParticleVertex));
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
  m_sprayIB = new IndexBuffer(sprayIndices, MaxSprayIndices);
  m_sprayVertexArray->setIndexBuffer(m_sprayIB);
  delete[] sprayIndices;

  m_sprayShader = new Shader("resources/default/shaders/SprayParticles.glsl");
}

void Renderer2d::drawBatches(const glm::mat4 &viewProjectionMatrix)
{
  PROFILE_FUNCTION();
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
                         : m_quadVertexArray->getIndexBuffer().getCount();
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
                         : m_textVertexArray->getIndexBuffer().getCount();
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
                          : m_sprayVertexArray->getIndexBuffer().getCount();
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
                        : m_triVertexArray->getIndexBuffer().getCount();
    glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

void Renderer2d::bindTextures()
{
  PROFILE_FUNCTION();
  for (uint32_t i = 0; i < m_textureSlotIndex; i++)
    m_textureSlots[i]->bindToSlot(i);
}

void Renderer2d::goBackToFirstVertex()
{
  PROFILE_FUNCTION();
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

void Renderer2d::uploadBasicUniforms(const glm::mat4 &viewProjectionMatrix,
                                     float globalTime,
                                     const glm::mat4 &transform)
{
  PROFILE_FUNCTION();
  m_quadTextureShader.bind();
  m_quadTextureShader.uploadUniformMat4("u_ViewProjection",
                                        viewProjectionMatrix);
  m_quadTextureShader.uploadUniformMat4("u_Transform", transform);

  m_triShader.bind();
  m_triShader.uploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
  m_triShader.uploadUniformMat4("u_Transform", transform);

  m_sprayShader.bind();
  m_sprayShader.uploadUniformMat4("u_ViewProjection", viewProjectionMatrix);
  m_sprayShader.uploadUniformMat4("u_Transform", transform);
  m_sprayShader.uploadUniformFloat("u_Time", globalTime);

  m_textTextureShader.bind();
  m_textTextureShader.uploadUniformMat4("u_ViewProjection",
                                        viewProjectionMatrix);
  m_textTextureShader.uploadUniformMat4("u_Transform", transform);
}

float Renderer2d::allocateTextures(Texture &texture)
{
  PROFILE_FUNCTION();
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
void Renderer2d::removeTexture(const Texture &texture)
{
  PROFILE_FUNCTION();
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
  PROFILE_FUNCTION();
  return glm::translate(glm::mat4(1.0f), {position, 0.f}) *
         glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
}
const glm::mat4 getTransform(const glm::vec2 &position, const glm::vec2 &size,
                             const float rotationAngleRadians)
{
  PROFILE_FUNCTION();
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
  PROFILE_FUNCTION();
  for (int i = 0; i < 4; i++) {
    m_quadVertexBufferPtr->position = transform * QuadVertexPositions[i];
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
  PROFILE_FUNCTION();
  for (int i = 0; i < 3; i++) {
    m_triVertexBufferPtr->position = transform * TriVertexPositions[i];
    m_triVertexBufferPtr->color = tintColor;
    m_triVertexBufferPtr++;
  }
  m_triIndexCount += 3;
}

void Renderer2d::beginSprayParticle(const float globalTime,
                                    const ParticleSprayComponent &psc)
{
  PROFILE_FUNCTION();
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
  PROFILE_FUNCTION();
  for (int i = 0; i < 4; i++) {
    m_sprayVertexBufferPtr->position = SprayVertexPositions[i];
    m_sprayVertexBufferPtr->offset = offset;
    m_sprayVertexBufferPtr->normal = normal;
    m_sprayVertexBufferPtr->time = startTime;
    m_sprayVertexBufferPtr->rotationSpeed = rotationSpeed;
    m_sprayVertexBufferPtr++;
  }
  m_sprayIndexCount += 6;
}

void Renderer2d::allocateCharacter(
    const glm::mat4 &transform, const glm::vec4 &tintColor,
    const std::array<glm::vec2, 4> &textureCoordinate,
    const std::array<glm::vec4, 4> &textVertexPositions)
{
  PROFILE_FUNCTION();
  for (int i = 0; i < 4; i++) {
    m_textVertexBufferPtr->position = transform * textVertexPositions[i];
    m_textVertexBufferPtr->color = tintColor;
    m_textVertexBufferPtr->texCoord = textureCoordinate[i];
    m_textVertexBufferPtr++;
  }
  m_textIndexCount += 6;
}

} // namespace pain
