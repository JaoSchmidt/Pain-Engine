// QuadBatch.cpp
#include "CoreRender/Renderer/BatchText.h"
#include "ContextBackend.h"
#include "Debugging/Profiling.h"

namespace pain
{

TextBatch TextBatch::create()
{
  // Quad indices
  std::vector<uint32_t> indices(MaxIndices);
  for (uint32_t i = 0, offset = 0; i < MaxIndices; i += 6, offset += 4) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;
    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
  }

  return TextBatch{
      *VertexBuffer::createVertexBuffer(
          MaxVertices * sizeof(TextQuadVertex),
          {
              {ShaderDataType::Float3, "a_Position"},
              {ShaderDataType::Float4, "a_Color"},
              {ShaderDataType::Float2, "a_TexCoord"},
          }),
      *IndexBuffer::createIndexBuffer(indices.data(), MaxIndices),
      *Shader::createFromFile("resources/default/shaders/Renderer2dText.glsl"),
  };
}
TextBatch::TextBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, ib)), shader(std::move(shader_)),
      cpuBuffer(std::make_unique<TextQuadVertex[]>(MaxVertices)),
      ptr(cpuBuffer.get()) {};

void TextBatch::reset()
{
  indexCount = 0;
  ptr = cpuBuffer.get();
}

void TextBatch::flush()
{
  if (!indexCount || !fontAtlas) {
    return;
  }

  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - cpuBuffer.get());
  vbo.setData(cpuBuffer.get(), count * sizeof(TextQuadVertex));

  fontAtlas->bind();

  shader.bind();
  shader.uploadUniformInt("u_FontAtlas", 0);

  ib.bind();
  backend::drawIndexed(vao, indexCount);
}

void TextBatch::allocateCharacter(
    const glm::mat4 &transform, const glm::vec4 &tintColor,
    const std::array<glm::vec2, 4> &textureCoordinate,
    const std::array<glm::vec4, 4> &textVertexPositions)
{
  PROFILE_FUNCTION();
  for (unsigned i = 0; i < 4; i++) {
    ptr->position = transform * textVertexPositions[i];
    ptr->color = tintColor;
    ptr->texCoord = textureCoordinate[i];
    ptr++;
  }
  indexCount += 6;
}

} // namespace pain
