#pragma once

#include "Core.h"

#include "CoreRender/Texture.h"
#include "CoreRender/VertexArray.h"
#include "Misc/BasicOrthoCamera.h"

namespace pain
{

class EXPORT Renderer2d
{
public:
  static void drawAndEndScene(const std::shared_ptr<VertexArray> &vertexArray);
  static void beginScene(const glm::mat4 &transform = glm::mat4(1.0f));
  static void endScene();

  static void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                       const glm::vec4 &color);
  static void drawQuad(const glm::vec2 &position, const glm::vec2 &size,
                       const std::shared_ptr<Texture> &texture,
                       float tilingFactor, const glm::vec4 &tintColor);

  static void init(OrthoCameraEntity *cameraEntity);
  static void shutdown();
  static void setViewport(int x, int y, int width, int height);
  static void setClearColor(const glm::vec4 &color);
  static void clear();
  static void drawIndexed(const std::shared_ptr<VertexArray> &vertexArray,
                          uint32_t indexCount = 0);

  static OrthoCameraEntity *m_cameraEntity;
  // static std::shared_ptr<OrthoCameraEntity> m_cameraEntity;

private:
  static void flush();
};

} // namespace pain
