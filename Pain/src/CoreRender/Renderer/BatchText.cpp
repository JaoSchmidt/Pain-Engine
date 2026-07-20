/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// QuadBatch.cpp
#include "CoreRender/Renderer/BatchText.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include "platform/ContextBackend.h"

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

  BufferLayout layout = {
      {ShaderDataType::Float3, "a_Position"},
      {ShaderDataType::UByte4, "a_Color", true},
      {ShaderDataType::Float2, "a_TexCoord"},
  };

  Shader shader =
      *Shader::createFromFile("resources/default/shaders/Renderer2dText.glsl");
  P_ASSERT(shader.verifyVertexLayout(layout),
           "TextBatch layout doesn't match shader '{}'", shader.getName());

  return TextBatch{
      *VertexBuffer::createVertexBuffer(MaxVertices * sizeof(TextQuadVertex),
                                        std::move(layout)),
      *IndexBuffer::createIndexBuffer(indices.data(), MaxIndices),
      std::move(shader),
  };
}
TextBatch::TextBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, ib)), shader(std::move(shader_)),
      cpuBuffer(std::make_unique<TextQuadVertex[]>(MaxVertices)),
      ptr(cpuBuffer.get()) {};

void TextBatch::resetPtr()
{
  indexCount = 0;
  ptr = cpuBuffer.get();
}
void TextBatch::resetAll()
{
  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void TextBatch::flush()
{
  if ((indexCount == 0) || (fontAtlas == nullptr)) {
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
#ifndef NDEBUG
  drawCount++;
#endif
}

void TextBatch::allocateCharacter(
    const glm::mat4 &transform, const Color &tintColor,
    const std::array<glm::vec2, 4> &textureCoordinate,
    const std::array<glm::vec4, 4> &textVertexPositions)
{
  PROFILE_FUNCTION();
  for (unsigned i = 0; i < 4; i++) {
    ptr->position = transform * textVertexPositions[i];
    ptr->color = tintColor.value;
    ptr->texCoord = textureCoordinate[i];
    ptr++;
  }
  indexCount += 6;
#ifndef NDEBUG
  statsCount++;
#endif
}

} // namespace pain
