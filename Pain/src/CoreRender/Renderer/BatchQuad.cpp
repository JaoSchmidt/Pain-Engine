// QuadBatch.cpp
#include "CoreRender/Renderer/BatchQuad.h"
#include "CoreFiles/LogWrapper.h"
#include "Debugging/Profiling.h"
#include <iostream>

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
              {ShaderDataType::UByte4, "a_Color", true},
              {ShaderDataType::Float2, "a_TexCoord"},
              {ShaderDataType::Float, "a_TexIndex"},
              {ShaderDataType::Float, "a_TilingFactor"},
          })),
      std::move(*IndexBuffer::createIndexBuffer(indices.data(), MaxIndices)),
      std::move(shader)};
}
QuadBatch::QuadBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader_)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),         //
      vao(*VertexArray::createVertexArray(vbo, ib)),    //
      shader(std::move(shader_)),                       //
      ptrInit(std::make_unique<Vertex[]>(MaxVertices)), //
      ptr(ptrInit.get())                                //
// drawOrder(std::vector<int>(MaxPolygons)),           //
// sortBuffer(std::make_unique<Vertex[]>(MaxVertices)) //
{};

void QuadBatch::resetPtr()
{
  indexCount = 0;
  ptr = ptrInit.get();
}
void QuadBatch::resetAll()
{
  resetPtr();
#ifndef NDEBUG
  statsCount = 0;
  drawCount = 0;
#endif
}

void QuadBatch::flush(const std::array<Texture *, MaxTextureSlots> &textures,
                      uint32_t textureCount)
{
  if (!indexCount)
    return;
  // sortByDrawOrder();
  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - ptrInit.get());
  vbo.setData(ptrInit.get(), count * sizeof(Vertex));

  for (uint32_t i = 0; i < textureCount; i++)
    textures[i]->bindToSlot(i);

  shader.bind();
  ib.bind();
  backend::drawIndexed(vao, indexCount * IndiceSize);
#ifndef NDEBUG
  drawCount++;
#endif
}

void QuadBatch::allocateQuad(const glm::mat4 &transform, const Color &tintColor,
                             const float tilingFactor, const float textureIndex,
                             const std::array<glm::vec2, 4> &textureCoordinate,
                             float order)
{
  PROFILE_FUNCTION();
  constexpr glm::vec4 QuadVertexPositions[4] = {
      glm::vec4(-0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, -0.5f, 0.f, 1.f),
      glm::vec4(0.5f, 0.5f, 0.f, 1.f),
      glm::vec4(-0.5f, 0.5f, 0.f, 1.f),
  };
  for (unsigned i = 0; i < 4; i++) {
    ptr->position =
        glm::vec3(glm::vec2(transform * QuadVertexPositions[i]), order);
    ptr->color = tintColor.value;
    ptr->texCoord = textureCoordinate[i];
    ptr->texIndex = textureIndex;
    ptr->tilingFactor = tilingFactor;
    ptr++;
  }
  // drawOrder[indexCount] = order;
  indexCount++;
#ifndef NDEBUG
  statsCount++;
#endif
}

// TODO: may be a bottleneck in the future. Can still be optimized with some
// more thought put into it
// void QuadBatch::sortByDrawOrder()
// {
//   auto printstuff = [](auto &v, const char *name) {
//     std::cout << name << std::endl;
//     for (unsigned i = 0; i < v.size(); i++) {
//       std::cout << v[i] << ", ";
//     }
//     std::cout << std::endl;
//   };
//
//   const uint32_t quadCount = indexCount;
//
//   // Create index indirection
//   std::vector<uint32_t> indices(quadCount);
//   for (uint32_t i = 0; i < quadCount; ++i) {
//     indices[i] = i;
//   }
//
//   printstuff(indices, "draw order 1");
//   // Sort indices by draw order
//   std::sort(indices.begin(), indices.end(), [this](uint32_t a, uint32_t b) {
//     return drawOrder[a] < drawOrder[b];
//   });
//
//   std::vector<uint32_t> newDrawOrder(quadCount);
//
//   for (uint32_t i = 0; i < quadCount; ++i) {
//     newDrawOrder[indices[i]] = i;
//   }
//   for (uint32_t i = 0; i < quadCount; ++i) {
//     const uint32_t correctIndex = newDrawOrder[i];
//
//     Vertex *a = sortBuffer.get() + correctIndex * VerticesPerQuad;
//     Vertex *b = ptrInit.get() + i * VerticesPerQuad;
//
//     for (uint32_t i = 0; i < VerticesPerQuad; ++i)
//       a[i] = b[i];
//
//     printstuff(indices, "draw order 2");
//   }
// }

} // namespace pain
