/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once
#include "pch.h"

#include "Core.h"

#include "Assets/ManagerTexture.h"
#include "CoreRender/Shader.h"
#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"

namespace pain
{

struct CubeVertex {
  glm::vec3 Position;
  glm::vec4 Color;
  glm::vec2 TexCoord;
};

class CubeVertexBatch
{
public:
  static CubeVertexBatch createCubeVertexBatch();
  const VertexArray &getVertexArray() const { return m_vertexArray; };
  VertexArray &getVertexArray() { return m_vertexArray; };
  const uint32_t &getIndexCount() const { return m_indexCount; };
  void goBackToFirst();
  void sendAllDataToOpenGL();
  void drawBatch(const glm::vec3 &position, const glm::vec3 &size,
                 const glm::vec4 &color);
  inline const Shader &getShader() const { return m_textureShader; };
  inline Shader &getShader() { return m_textureShader; };

private:
  CubeVertexBatch(VertexArray m_vertexArray,
                  VertexBuffer m_vertexBuffer, //
                  Shader m_textureShader,      //
                  CubeVertex *m_vertexBufferBase);
  static constexpr uint32_t MaxCubes = 1000;
  static constexpr uint32_t MaxVertices = MaxCubes * 8;
  static constexpr uint32_t MaxIndices = MaxCubes * 36;

  VertexArray m_vertexArray;
  VertexBuffer m_vertexBuffer;
  Shader m_textureShader;
  Texture &m_whiteTexture =
      TextureManager::getDefaultTexture(TextureManager::DefaultTexture::Blank);

  CubeVertex *m_vertexBufferBase = nullptr;
  CubeVertex *m_vertexBufferPtr = nullptr;
  uint32_t m_indexCount = 0;
};

} // namespace pain
