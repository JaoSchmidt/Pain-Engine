// QuadBatch.cpp
#include "CoreRender/Renderer/BatchQuad.h"
#include "Debugging/Profiling.h"

namespace pain
{

QuadBatch QuadBatch::create()
{
  std::vector<uint32_t> indices(MaxIndices);
  for (uint32_t i = 0, offset = 0; i < MaxIndices; i += 6, offset += 4) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;

    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
  }

  Shader shader =
      *Shader::createFromFile("resources/default/shaders/Texture.glsl");

  // Set texture samplers once
  int samplers[MaxTextureSlots];
  for (int i = 0; i < static_cast<int>(MaxTextureSlots); i++)
    samplers[i] = i;

  shader.bind();
  shader.uploadUniformIntArray("u_Textures", samplers, MaxTextureSlots);
  return QuadBatch{
      std::move(*VertexBuffer::createVertexBuffer(
          MaxVertices * sizeof(Vertex),
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::Float4, "a_Color"},
              {ShaderDataType::Float2, "a_TexCoord"},
              {ShaderDataType::Float, "a_TexIndex"},
              {ShaderDataType::Float, "a_TilingFactor"},
          })),
      std::move(*IndexBuffer::createIndexBuffer(indices.data(), MaxIndices)),
      std::move(shader)};
}
QuadBatch::QuadBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, ib)), shader(std::move(shader_)),
      cpuBuffer(std::make_unique<Vertex[]>(MaxVertices)),
      ptr(cpuBuffer.get()) {};
void QuadBatch::reset()
{
  indexCount = 0;
  ptr = cpuBuffer.get();
}

void QuadBatch::flush(const std::array<Texture *, MaxTextureSlots> &textures,
                      uint32_t textureCount)
{
  if (!indexCount)
    return;

  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - cpuBuffer.get());
  vbo.setData(cpuBuffer.get(), count * sizeof(Vertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  shader.bind();
  ib.bind();
  backend::drawIndexed(vao, indexCount);
}

void QuadBatch::allocateQuad(const glm::mat4 &transform,
                             const glm::vec4 &tintColor,
                             const float tilingFactor, const float textureIndex,
                             const std::array<glm::vec2, 4> &textureCoordinate)
{
  PROFILE_FUNCTION();
  constexpr glm::vec4 QuadVertexPositions[4] = {
      glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, 0.5f, 0.f, 1.f),
      glm::vec4(-0.5f, 0.5f, 0.f, 1.f),
  };
  for (unsigned i = 0; i < 4; i++) {
    ptr->position = transform * QuadVertexPositions[i];
    ptr->color = tintColor;
    ptr->texCoord = textureCoordinate[i];
    ptr->texIndex = textureIndex;
    ptr->tilingFactor = tilingFactor;
    ptr++;
  }
  indexCount += 6;
}

} // namespace pain
