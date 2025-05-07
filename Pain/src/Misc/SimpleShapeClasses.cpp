module;
#include <cstdint>
#include <memory>
module pain.SimpleShapeClasses;
import pain.AllBuffers;
import pain.BufferLayout;
import pain.VertexArray;

namespace pain
{

IsocelesTriangle::IsocelesTriangle(float w, float h)
{

  const float RED_TRIANGLE_Z = 0.0f;
  // clang-format off
  float vertices[3 * 7] = {
      -w/2, -w/2, RED_TRIANGLE_Z, +0.8f, +0.2f, +0.8f, 1.0f,
       w/2, -w/2, RED_TRIANGLE_Z, +0.2f, +0.3f, +0.8f, 1.0f,
         0,    h, RED_TRIANGLE_Z, +0.8f, +0.8f, +0.2f, 1.0f,
  };
  // clang-format on
  std::shared_ptr<VertexBuffer> vb;
  vb.reset(new VertexBuffer(vertices, sizeof(vertices)));
  vb->setLayout({{ShaderDataType::Float3, "a_Position"},
                 {ShaderDataType::Float4, "a_Color"}});

  uint32_t indices[3] = {
      0, 1, 2, //
  };
  std::shared_ptr<IndexBuffer> ib;
  ib.reset(new IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));

  m_vertexArray.reset(new VertexArray());
  m_vertexArray->addVertexBuffer(vb);
  m_vertexArray->setIndexBuffer(ib);
};

Rectangle::Rectangle()
{
  float squareVertices[3 * 4] = {
      -0.5f, -0.5f, 0.0f, //
      0.5f,  -0.5f, 0.0f, //
      0.5f,  0.5f,  0.0f, //
      -0.5f, 0.5f,  0.0f  //
  };

  std::shared_ptr<VertexBuffer> vb;
  vb.reset(new VertexBuffer(squareVertices, sizeof(squareVertices)));
  vb->setLayout({{ShaderDataType::Float3, "a_Position"}});

  uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
  std::shared_ptr<IndexBuffer> ib;
  ib.reset(
      new IndexBuffer(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

  m_vertexArray.reset(new VertexArray());
  m_vertexArray->addVertexBuffer(vb);
  m_vertexArray->setIndexBuffer(ib);
}

TextureRectangle::TextureRectangle()
{
  float squareVertices[5 * 4] = {
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, //
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, //
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, //
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  //
  };

  std::shared_ptr<VertexBuffer> vb;
  vb.reset(new VertexBuffer(squareVertices, sizeof(squareVertices)));
  vb->setLayout({{ShaderDataType::Float3, "a_Position"},
                 {ShaderDataType::Float2, "a_TexCoord"}});

  uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
  std::shared_ptr<IndexBuffer> ib;
  ib.reset(
      new IndexBuffer(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

  m_vertexArray.reset(new VertexArray());
  m_vertexArray->addVertexBuffer(vb);
  m_vertexArray->setIndexBuffer(ib);
}
} // namespace pain
