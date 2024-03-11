#include "Misc/SimpleShapeClasses.h"
#include "CoreRender/AllBuffers.h"
#include "CoreRender/BufferLayout.h"
#include "CoreRender/VertexArray.h"

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
  vb->setLayout({{pain::ShaderDataType::Float3, "a_Position"},
                 {pain::ShaderDataType::Float4, "a_Color"}});

  uint32_t indices[3] = {
      0, 1, 2, //
  };
  std::shared_ptr<IndexBuffer> ib;
  ib.reset(new IndexBuffer(indices, sizeof(indices) / sizeof(uint32_t)));

  m_vertexArray.reset(new VertexArray());
  m_vertexArray->addVertexBuffer(vb);
  m_vertexArray->setIndexBuffer(ib);
  std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

  std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec4 v_Color;
			in vec3 v_Position;

			void main()
			{
				color = v_Color;
			}
		)";

  m_shader.reset(new Shader(vertexSrc, fragmentSrc));
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

  std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

  std::string fragmSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			
			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

  m_shader.reset(new Shader(vertexSrc, fragmSrc));
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

  std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			out vec2 v_TexCoord;
			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

  std::string fragmSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec2 v_TexCoord;
			
			uniform sampler2D u_Texture;
			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
		)";

  m_shader.reset(new Shader(vertexSrc, fragmSrc));
}
} // namespace pain
