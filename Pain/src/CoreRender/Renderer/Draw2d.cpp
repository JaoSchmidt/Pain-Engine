#include "CoreFiles/LogWrapper.h"
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

Renderer2d Renderer2d::createRenderer2d()
{

  PROFILE_FUNCTION();
  // NOTE: This enable 3d and can be changed later in case we need some camera
  // mechanic
  // glEnable(GL_DEPTH_TEST);
  // =============================================================== //
  // Quads
  // =============================================================== //

  auto quadVertexBuffer = VertexBuffer::createVertexBuffer(
      MaxQuadVertices * sizeof(QuadVertex),
      {{ShaderDataType::Float3, "a_Position"}, //
       {ShaderDataType::Float4, "a_Color"},    //
       {ShaderDataType::Float2, "a_TexCoord"}, //
       {ShaderDataType::Float, "a_TexIndex"},  //
       {ShaderDataType::Float, "a_TilingFactor"}});

  P_ASSERT(quadVertexBuffer, "Could not create Quad Vertex BUffer");
  QuadVertex *quadVertexBufferBase = new QuadVertex[MaxQuadVertices];

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
  auto quadIB = IndexBuffer::createIndexBuffer(quadIndices, MaxQuadIndices);
  P_ASSERT(quadIB, "Could not create Quad Vertex BUffer");
  delete[] quadIndices;
  auto quadVertexArray =
      VertexArray::createVertexArray(quadVertexBuffer.value(), *quadIB);
  P_ASSERT(quadVertexArray, "Quad Vertex Array wasn't initialized");

  int32_t samplers[MaxTextureSlots];
  for (uint32_t i = 0; i < MaxTextureSlots; i++) {
    samplers[i] = i;
  }

  auto quadTextureShader =
      Shader::createFromFile("resources/default/shaders/Texture.glsl");
  P_ASSERT(quadTextureShader, "Quad shader wasn't initialized");
  quadTextureShader->bind();
  quadTextureShader->uploadUniformIntArray("u_Textures", samplers,
                                           MaxTextureSlots);
  // =============================================================== //
  // Triangles
  // =============================================================== //

  auto triVertexBuffer =
      VertexBuffer::createVertexBuffer(MaxTriVertices * sizeof(TriVertex),
                                       {{ShaderDataType::Float3, "a_Position"},
                                        {ShaderDataType::Float4, "a_Color"}});
  TriVertex *triVertexBufferBase = new TriVertex[MaxTriVertices];

  // indices
  uint32_t *triIndices = new uint32_t[MaxTriIndices];
  for (uint32_t i = 0, offset = 0; i < MaxTriIndices; i += 3, offset += 3) {
    triIndices[i + 0] = offset + 0;
    triIndices[i + 1] = offset + 1;
    triIndices[i + 2] = offset + 2;
  }
  auto triIB = IndexBuffer::createIndexBuffer(triIndices, MaxTriIndices);
  P_ASSERT(triIB, "Could not create Triangle Vertex BUffer");
  delete[] triIndices;

  auto triShader =
      Shader::createFromFile("resources/default/shaders/Triangles.glsl");
  P_ASSERT(triShader, "Triangule shader wasn't initialized");
  auto triVertexArray =
      VertexArray::createVertexArray(*triVertexBuffer, *triIB);
  // =============================================================== //
  // Text
  // =============================================================== //

  auto textVertexBuffer = VertexBuffer::createVertexBuffer(
      MaxQuadVertices * sizeof(TextQuadVertex),
      {
          {ShaderDataType::Float3, "a_Position"}, //
          {ShaderDataType::Float4, "a_Color"},    //
          {ShaderDataType::Float2, "a_TexCoord"}, //
      });
  TextQuadVertex *textVertexBufferBase = new TextQuadVertex[MaxQuadVertices];

  auto textTextureShader =
      Shader::createFromFile("resources/default/shaders/Renderer2dText.glsl");
  textTextureShader->bind();
  P_ASSERT(textTextureShader, "Triangule shader wasn't initialized");
  auto textVertexArray =
      VertexArray::createVertexArray(*textVertexBuffer, *quadIB);

  // =============================================================== //
  // Spray Particles
  // =============================================================== //

  auto sprayVertexBuffer = VertexBuffer::createVertexBuffer(
      MaxSprayVertices * sizeof(ParticleVertex),
      {{ShaderDataType::Float2, "a_Position"}, //
       {ShaderDataType::Float2, "a_Offset"},
       {ShaderDataType::Float2, "a_Normal"},
       {ShaderDataType::Float, "a_Time"},
       {ShaderDataType::Float, "a_RotationSpeed"}});
  ParticleVertex *sprayVertexBufferBase = new ParticleVertex[MaxSprayVertices];

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
  auto sprayIB = IndexBuffer::createIndexBuffer(sprayIndices, MaxSprayIndices);
  delete[] sprayIndices;

  auto sprayVertexArray =
      VertexArray::createVertexArray(*sprayVertexBuffer, *sprayIB);
  auto sprayShader =
      Shader::createFromFile("resources/default/shaders/SprayParticles.glsl");

  // =============================================================== //
  // Create Renderer
  // =============================================================== //
  // allow transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // HACK: allow textures with 3 channels to align properly, e.g. font textures.
  // No idea why it works tho, perhaps I will find a proper doc later
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  return Renderer2d(std::move(*quadIB),            //
                    std::move(*triIB),             //
                    std::move(*sprayIB),           //
                    std::move(*quadVertexArray),   //
                    std::move(*quadVertexBuffer),  //
                    std::move(*quadTextureShader), //
                    quadVertexBufferBase,
                    // text initializer
                    std::move(*textVertexArray),
                    std::move(*textVertexBuffer),  //
                    std::move(*textTextureShader), //
                    textVertexBufferBase,
                    // tri initializer
                    std::move(*triVertexArray),
                    std::move(*triVertexBuffer), //
                    std::move(*triShader),       //
                    triVertexBufferBase,
                    // spray particle initializer
                    std::move(*sprayVertexArray),
                    std::move(*sprayVertexBuffer), //
                    std::move(*sprayShader),       //
                    sprayVertexBufferBase);
}

void Renderer2d::drawBatches(const glm::mat4 &viewProjectionMatrix)
{
  PROFILE_FUNCTION();
  // =============================================================== //
  // Quads
  // =============================================================== //
  // PLOG_I("bind texture id = {}", m_textureSlots[i]->getRendererId());
  if (m_quadIndexCount) {
    m_quadIB.bind(); // TODO: this fixed the issue, but why?
    m_quadVertexArray.bind();
    const uint32_t quadDataSize =
        (uint8_t *)m_quadVertexBufferPtr - (uint8_t *)m_quadVertexBufferBase;
    m_quadVertexBuffer.setData((void *)m_quadVertexBufferBase, quadDataSize);

    // bind textures
    for (uint32_t i = 0; i < m_textureSlotIndex; i++) {
      m_textureSlots[i]->bindToSlot(i);
    }

    m_quadTextureShader.bind();
    const uint32_t quadCount =
        m_quadIndexCount ? m_quadIndexCount
                         : m_quadVertexArray.getIndexBuffer().getCount();
    glDrawElements(GL_TRIANGLES, quadCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  // =============================================================== //
  // Text
  // =============================================================== //
  if (m_textIndexCount) {
    m_textVertexArray.bind();
    const uint32_t textDataSize =
        (uint8_t *)m_textVertexBufferPtr - (uint8_t *)m_textVertexBufferBase;
    m_textVertexBuffer.setData((void *)m_textVertexBufferBase, textDataSize);

    m_fontAtlasTexture->bind();
    m_textTextureShader.bind();
    m_textTextureShader.uploadUniformInt("u_FontAtlas", 0);

    const uint32_t textCount =
        m_textIndexCount ? m_textIndexCount
                         : m_textVertexArray.getIndexBuffer().getCount();
    glDrawElements(GL_TRIANGLES, textCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // =============================================================== //
  // Spray Particles
  // =============================================================== //
  if (m_sprayIndexCount) {
    m_sprayVertexArray.bind();
    const uint32_t sprayDataSize =
        (uint8_t *)m_sprayVertexBufferPtr - (uint8_t *)m_sprayVertexBufferBase;
    m_sprayVertexBuffer.setData((void *)m_sprayVertexBufferBase, sprayDataSize);

    // FIX: I don't really need to bind textures here, right?
    // TODO: make it possible to bind textures for spray particles
    for (uint32_t i = 0; i < m_textureSlotIndex; i++)
      m_textureSlots[i]->bindAndClearSlot();

    m_sprayShader.bind();
    const uint32_t sprayCount =
        m_sprayIndexCount ? m_sprayIndexCount
                          : m_sprayVertexArray.getIndexBuffer().getCount();
    glDrawElements(GL_TRIANGLES, sprayCount, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  // =============================================================== //
  // Triangles
  // =============================================================== //
  if (m_triIndexCount) {
    m_triVertexArray.bind();
    const uint32_t triDataSize =
        (uint8_t *)m_triVertexBufferPtr - (uint8_t *)m_triVertexBufferBase;
    m_triVertexBuffer.setData((void *)m_triVertexBufferBase, triDataSize);

    m_triShader.bind();
    const uint32_t triCount =
        m_triIndexCount ? m_triIndexCount
                        : m_triVertexArray.getIndexBuffer().getCount();
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
const glm::mat4 Renderer2d::getTransform(const glm::vec2 &position,
                                         const glm::vec2 &size)
{
  PROFILE_FUNCTION();
  return glm::translate(glm::mat4(1.0f), {position, 0.f}) *
         glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
}
const glm::mat4 Renderer2d::getTransform(const glm::vec2 &position,
                                         const glm::vec2 &size,
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
  m_sprayShader.bind();
  m_sprayShader.uploadUniformFloat("u_Time", globalTime);
  m_sprayShader.uploadUniformFloat("u_ParticleVelocity", psc.m_velocity);
  m_sprayShader.uploadUniformFloat("u_LifeTime", psc.m_lifeTime);
  m_sprayShader.uploadUniformFloat("u_SizeChangeSpeed", psc.m_sizeChangeSpeed);
  m_sprayShader.uploadUniformFloat("u_randomSizeFactor", psc.m_randSizeFactor);
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
Renderer2d::~Renderer2d()
{
  delete[] m_quadVertexBufferBase;
  delete[] m_textVertexBufferBase;
  delete[] m_triVertexBufferBase;
  delete[] m_sprayVertexBufferBase;
}

Renderer2d::Renderer2d(IndexBuffer quadIB,            //
                       IndexBuffer triIB,             //
                       IndexBuffer sprayIB,           //
                       VertexArray quadVertexArray,   //
                       VertexBuffer quadVertexBuffer, //
                       Shader quadTextureShader,
                       QuadVertex *quadVertexBufferBase,
                       // text initializer
                       VertexArray textVertexArray,
                       VertexBuffer textVertexBuffer, //
                       Shader textTextureShader,
                       TextQuadVertex *textVertexBufferBase,
                       // tri initializer
                       VertexArray triVertexArray,
                       VertexBuffer triVertexBuffer, //
                       Shader triShader,             //
                       TriVertex *triVertexBufferBase,
                       // spray particle initializer
                       VertexArray sprayVertexArray,
                       VertexBuffer sprayVertexBuffer, //
                       Shader sprayShader,
                       ParticleVertex *sprayVertexBufferBase)
    : m_quadVertexArray(std::move(quadVertexArray)),
      m_quadVertexBuffer(std::move(quadVertexBuffer)),
      m_quadTextureShader(std::move(quadTextureShader)),
      m_quadVertexBufferBase(quadVertexBufferBase),
      m_textVertexArray(std::move(textVertexArray)),
      m_textVertexBuffer(std::move(textVertexBuffer)),
      m_textTextureShader(std::move(textTextureShader)),
      m_textVertexBufferBase(textVertexBufferBase),
      m_triVertexArray(std::move(triVertexArray)),
      m_triVertexBuffer(std::move(triVertexBuffer)),
      m_triShader(std::move(triShader)),
      m_triVertexBufferBase(triVertexBufferBase),
      m_sprayVertexArray(std::move(sprayVertexArray)),
      m_sprayVertexBuffer(std::move(sprayVertexBuffer)),
      m_sprayShader(std::move(sprayShader)), //
      m_sprayVertexBufferBase(sprayVertexBufferBase),
      m_textureSlots({&resources::getDefaultTexture(resources::BLANK)}),
      m_quadIB(std::move(quadIB)), //
      m_triIB(std::move(triIB)),   //
      m_sprayIB(std::move(sprayIB)) {};

Renderer2d &Renderer2d::operator=(Renderer2d &&other) noexcept
{
  if (this == &other)
    return *this;

  // Move GPU resources
  m_quadVertexArray = std::move(other.m_quadVertexArray);
  m_quadVertexBuffer = std::move(other.m_quadVertexBuffer);
  m_quadTextureShader = std::move(other.m_quadTextureShader);
  m_quadVertexBufferBase = other.m_quadVertexBufferBase;
  m_quadVertexBufferPtr = other.m_quadVertexBufferPtr;
  m_quadIndexCount = other.m_quadIndexCount;

  m_textVertexArray = std::move(other.m_textVertexArray);
  m_textVertexBuffer = std::move(other.m_textVertexBuffer);
  m_textTextureShader = std::move(other.m_textTextureShader);
  m_textVertexBufferBase = other.m_textVertexBufferBase;
  m_textVertexBufferPtr = other.m_textVertexBufferPtr;
  m_textIndexCount = other.m_textIndexCount;

  m_triVertexArray = std::move(other.m_triVertexArray);
  m_triVertexBuffer = std::move(other.m_triVertexBuffer);
  m_triShader = std::move(other.m_triShader);
  m_triVertexBufferBase = other.m_triVertexBufferBase;
  m_triVertexBufferPtr = other.m_triVertexBufferPtr;
  m_triIndexCount = other.m_triIndexCount;

  m_sprayVertexArray = std::move(other.m_sprayVertexArray);
  m_sprayVertexBuffer = std::move(other.m_sprayVertexBuffer);
  m_sprayShader = std::move(other.m_sprayShader);
  m_sprayVertexBufferBase = other.m_sprayVertexBufferBase;
  m_sprayVertexBufferPtr = other.m_sprayVertexBufferPtr;
  m_sprayIndexCount = other.m_sprayIndexCount;

  // Texture handling
  m_whiteTexture = other.m_whiteTexture;
  m_textureSlots = std::move(other.m_textureSlots);
  m_textureSlotIndex = other.m_textureSlotIndex;

  // Index buffers
  m_quadIB = std::move(other.m_quadIB);
  m_triIB = std::move(other.m_triIB);
  m_sprayIB = std::move(other.m_sprayIB);

  // Camera & font
  m_fontAtlasTexture = other.m_fontAtlasTexture;
  m_cameraMatrices = other.m_cameraMatrices;

  // Reset other's raw pointers & counts to safe state
  other.m_quadVertexBufferBase = nullptr;
  other.m_quadVertexBufferPtr = nullptr;
  other.m_quadIndexCount = 0;

  other.m_textVertexBufferBase = nullptr;
  other.m_textVertexBufferPtr = nullptr;
  other.m_textIndexCount = 0;

  other.m_triVertexBufferBase = nullptr;
  other.m_triVertexBufferPtr = nullptr;
  other.m_triIndexCount = 0;

  other.m_sprayVertexBufferBase = nullptr;
  other.m_sprayVertexBufferPtr = nullptr;
  other.m_sprayIndexCount = 0;

  other.m_textureSlotIndex = 1;
  other.m_whiteTexture = nullptr;
  other.m_fontAtlasTexture = nullptr;
  other.m_cameraMatrices = nullptr;

  return *this;
}

} // namespace pain
