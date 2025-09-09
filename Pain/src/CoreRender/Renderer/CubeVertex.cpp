#include "CoreRender/Renderer/CubeVertex.h"

namespace pain
{
void CubeVertexBatch::goBackToFirst()
{
  m_indexCount = 0;
  m_vertexBufferPtr = m_vertexBufferBase;
}
void CubeVertexBatch::sendAllDataToOpenGL()
{
  uint32_t dataSize =
      (uint8_t *)m_vertexBufferPtr - (uint8_t *)m_vertexBufferBase;
  m_vertexBuffer->setData((void *)m_vertexBufferBase, dataSize);
}

/* Batch logic mostly in this function */
void CubeVertexBatch::drawBatch(const glm::vec3 &position,
                                const glm::vec3 &size, const glm::vec4 &color)
{
  glm::vec4 Color = glm::vec4(0.9f, 0.3f, 0.2f, 1.0f);
  // first quad
  m_vertexBufferPtr->Position = {position.x, position.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y + size.y,
                                 position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 1.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x, position.y + size.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 1.0f};
  m_vertexBufferPtr++;
  m_indexCount += 6;

  Color = glm::vec4(0.3f, 0.9f, 0.2f, 1.0f);
  // second quad
  m_vertexBufferPtr->Position = {position.x, position.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x, position.y, position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x, position.y + size.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 1.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x, position.y + size.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 1.0f};
  m_vertexBufferPtr++;
  m_indexCount += 6;

  // third quad
  Color = glm::vec4(0.3f, 0.2f, 0.9f, 1.0f);
  m_vertexBufferPtr->Position = {position.x, position.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x, position.y, position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 1.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 1.0f};
  m_vertexBufferPtr++;
  m_indexCount += 6;

  // forth quad
  Color = glm::vec4(0.8f, 0.1f, 0.8f, 1.0f);
  m_vertexBufferPtr->Position = {position.x + size.x, position.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y + size.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 1.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y + size.y,
                                 position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 1.0f};
  m_vertexBufferPtr++;
  m_indexCount += 6;

  // fifth quad
  Color = glm::vec4(0.1f, 0.8f, 0.8f, 1.0f);
  m_vertexBufferPtr->Position = {position.x, position.y, position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y + size.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 1.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x, position.y + size.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 1.0f};
  m_vertexBufferPtr++;
  m_indexCount += 6;

  // sixth quad
  Color = glm::vec4(0.8f, 0.8f, 0.1f, 1.0f);
  m_vertexBufferPtr->Position = {position.x, position.y + size.y, position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y + size.y,
                                 position.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 0.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x + size.x, position.y + size.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {1.0f, 1.0f};
  m_vertexBufferPtr++;

  m_vertexBufferPtr->Position = {position.x, position.y + size.y,
                                 position.z + size.z};
  m_vertexBufferPtr->Color = Color;
  m_vertexBufferPtr->TexCoord = {0.0f, 1.0f};
  m_vertexBufferPtr++;
  m_indexCount += 6;
}

CubeVertexBatch::CubeVertexBatch()
{
  m_vertexArray = new VertexArray();

  m_vertexBuffer = new VertexBuffer(MaxVertices * sizeof(CubeVertex));
  m_vertexBuffer->setLayout({
      {ShaderDataType::Float3, "a_Position"}, //
      {ShaderDataType::Float4, "a_Color"},    //
      {ShaderDataType::Float2, "a_TexCoord"}  //
  });
  m_vertexArray->addVertexBuffer(m_vertexBuffer);

  m_vertexBufferBase = new CubeVertex[MaxVertices];

  uint32_t *quadIndices = new uint32_t[MaxIndices];

  uint32_t offset = 0;
  for (uint32_t i = 0; i < MaxIndices; i += 6) {
    quadIndices[i + 0] = offset + 0;
    quadIndices[i + 1] = offset + 1;
    quadIndices[i + 2] = offset + 2;

    quadIndices[i + 3] = offset + 2;
    quadIndices[i + 4] = offset + 3;
    quadIndices[i + 5] = offset + 0;

    offset += 4;
  }

  // NOTE: indexbuffer will need to be replace to some shared memory because it
  // will on more then one buffer
  m_vertexArray->setIndexBuffer(new IndexBuffer(quadIndices, MaxIndices));
  delete[] quadIndices;

  // m_whiteTexture.reset(new Texture(1, 1));
  // uint32_t whiteTextureData = 0xffffffff;
  // m_whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

  m_textureShader = new Shader("resources/shaders/Texture.glsl");
  // m_textureShader->bind();
  // m_textureShader->uploadUniformInt("u_Texture", 0);
}

} // namespace pain
