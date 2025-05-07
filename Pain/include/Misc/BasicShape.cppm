module;
#include <SDL_events.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
export module pain.BasicShape;
import pain.Movement;
import pain.GameObject;
import pain.Sprite;
import pain.Texture;
import pain.Scene;

export namespace pain
{

class RectangleSprite : public GameObject<TransformComponent, SpriteComponent>
{
public:
  RectangleSprite(Scene *scene, const glm::vec2 &position,
                  const glm::vec2 &size, const glm::vec4 &color,
                  Texture *ptexture, float tilingFactor);

  void onUpdate(double dt) {};
  void onEvent(const SDL_Event &e) {};
};

class Rectangle : public GameObject<TransformComponent, SpriteComponent>
{
public:
  Rectangle(Scene *scene, const glm::vec2 &position, const glm::vec2 &size,
            const glm::vec4 &color);
  void onUpdate(double dt) {};
  void onEvent(const SDL_Event &e) {};
};

} // namespace pain
