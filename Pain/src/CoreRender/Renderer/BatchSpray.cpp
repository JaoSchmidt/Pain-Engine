#include "CoreRender/Renderer/BatchSpray.h"
#include "ContextBackend.h"
#include "Debugging/Profiling.h"
namespace pain
{

SprayBatch SprayBatch::create()
{
  // Indices (quad-style)
  std::vector<uint32_t> indices(MaxIndices);
  for (uint32_t i = 0, offset = 0; i < MaxIndices; i += 6, offset += 4) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;

    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
  }

  return SprayBatch{
      *VertexBuffer::createVertexBuffer(
          MaxVertices * sizeof(ParticleVertex),
          {
              {ShaderDataType::Float2, "a_Position"},
              {ShaderDataType::Float2, "a_Offset"},
              {ShaderDataType::Float2, "a_Normal"},
              {ShaderDataType::Float, "a_Time"},
              {ShaderDataType::Float, "a_RotationSpeed"},
          }),
      *IndexBuffer::createIndexBuffer(indices.data(), MaxIndices),
      *Shader::createFromFile("resources/default/shaders/SprayParticles.glsl"),
  };
}
SprayBatch::SprayBatch(VertexBuffer &&vbo_, IndexBuffer &&ib_, Shader &&shader)
    : vbo(std::move(vbo_)), ib(std::move(ib_)),
      vao(*VertexArray::createVertexArray(vbo, ib)), shader(std::move(shader)),
      cpuBuffer(std::make_unique<ParticleVertex[]>(MaxVertices)),
      ptr(cpuBuffer.get()) {};

void SprayBatch::reset()
{
  indexCount = 0;
  ptr = cpuBuffer.get();
}

void SprayBatch::flush()
{
  if (!indexCount)
    return;

  vao.bind();
  vbo.bind();

  const uint32_t count = static_cast<uint32_t>(ptr - cpuBuffer.get());
  vbo.setData(cpuBuffer.get(), count * sizeof(Vertex));

  shader.bind();
  ib.bind();
  backend::drawIndexed(vao, indexCount);
}

void SprayBatch::allocateSprayParticles(const glm::vec2 &position,
                                        const glm::vec2 &offset,
                                        const glm::vec2 &normal,
                                        const DeltaTime startTime,
                                        const float rotationSpeed)
{
  UNUSED(position)
  PROFILE_FUNCTION();
  constexpr glm::vec2 SprayVertexPositions[4] = {
      glm::vec2(-0.5f, -0.5f),
      glm::vec2(0.5f, -0.5f),
      glm::vec2(0.5f, 0.5f),
      glm::vec2(-0.5f, 0.5f),
  };
  for (unsigned i = 0; i < 4; i++) {
    ptr->position = SprayVertexPositions[i];
    ptr->offset = offset;
    ptr->normal = normal;
    ptr->time = startTime.getSecondsf();
    ptr->rotationSpeed = rotationSpeed;
    ptr++;
  }
  indexCount += 6;
}

} // namespace pain
