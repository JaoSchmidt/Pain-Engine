#pragma once
#include "CoreRender/Buffers/Material.h"
#include "CoreRender/Renderer/Misc.h"
#include "Font.h"

class RenderContext;

namespace pain
{

struct RenderCommand {
  RenderCommandType m_type;

  union Data {
    struct {
      const Material *material;
      glm::mat4 transform;
    } mesh;
    struct {
      const Material *material;
      glm::mat4 transform;
      RenderLayer layer;
    } sprite;
    struct {
      const Material *material;
      glm::mat4 transform;
      SphereDivision div;
    } sphere;
    struct {
      glm::vec3 a;
      glm::vec3 b;
    } line3d;
    struct {
      const Material *material;
      glm::vec2 origin;
      glm::vec2 destination;
      float thickness;
      RenderLayer layer;
    } line;
    struct {
      glm::vec2 position;
      float scale;
      const char *string;
      const Font *font;
      Color color;
    } text;

    struct LightPoint {
      glm::vec3 position;
      Color color;
    } lightPoint;
  } m_data;
};

} // namespace pain
