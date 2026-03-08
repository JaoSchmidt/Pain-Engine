#pragma once
#include "CoreRender/Buffers/Material.h"
#include "CoreRender/Renderer/Misc.h"
#include "ECS/Components/Sprite.h"

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
      const SpriteComponent *sprite;
      glm::mat4 transform;
    } sprite;
    struct {
      glm::vec3 a;
      glm::vec3 b;
    } line;
    struct LightPoint {
      glm::vec3 position;
      Color color;
    } lightPoint;
  } m_data;
};

} // namespace pain
