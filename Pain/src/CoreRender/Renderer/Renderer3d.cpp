/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */



#include "CoreRender/Renderer/Renderer3d.h"
#include "CoreFiles/LogWrapper.h"
#include "CoreRender/Renderer/CubeVertex.h"
#include "CoreRender/Texture.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

namespace pain
{

CubeVertexBatch *Renderer3d::m_cubeBatch = nullptr;
std::shared_ptr<PerspectiveCameraController> Renderer3d::m_cameraController =
    nullptr;

// ================================================================= //
// Render initialization and destruction
// ================================================================= //

void Renderer3d::init(
    std::shared_ptr<PerspectiveCameraController> &cameraController)
{
  // m_cubeBatch = new CubeVertexBatch();
  // NOTE: This can be changed later in case the engine needs a camera mechanic

  glEnable(GL_DEPTH_TEST);
  m_cameraController = cameraController;
}

void Renderer3d::shutdown() { delete m_cubeBatch; }

// ================================================================= //
// Draws
// ================================================================= //

void Renderer3d::drawCube(const glm::vec3 &position, const glm::vec3 &size,
                          const glm::vec4 &color)
{
  m_cubeBatch->drawBatch(position, size, color);
}

// ================================================================= //
// Renderer basic wrapper around opengl
// ================================================================= //

void Renderer3d::setViewport(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
}

void Renderer3d::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void Renderer3d::setClearColor(const glm::vec4 color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer3d::beginScene(const glm::mat4 &transform)
{
  m_cubeBatch->getShader().bind();
  m_cubeBatch->getShader().uploadUniformMat4(
      "u_ViewProjection",
      m_cameraController->getCamera().getViewProjectionMatrix());
  m_cubeBatch->getShader().uploadUniformMat4("u_Transform", transform);

  m_cubeBatch->goBackToFirst();
}

void Renderer3d::endScene()
{
  m_cubeBatch->sendAllDataToOpenGL();
  // NOTE: this probably won't be here in the future,
  // otherwise flush() wouldn't need to be a function
  flush();
}

void Renderer3d::flush()
{
  drawIndexed(m_cubeBatch->getVertexArray(), m_cubeBatch->getIndexCount());
}

void Renderer3d::drawIndexed(VertexArray &vertexArray, uint32_t indexCount)
{
  uint32_t count =
      indexCount ? indexCount : vertexArray.getIndexBuffer().getCount();
  glDrawElements(GL_TRIANGLES, static_cast<int32_t>(count), GL_UNSIGNED_INT,
                 nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace pain
