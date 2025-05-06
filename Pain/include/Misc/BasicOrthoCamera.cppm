module;
#include "SDL_events.h"
#include "glm/fwd.hpp"
export module pain.BasicOrthoCamera;
import pain.Camera;
import pain.Scene;
import pain.CameraComponent;
import pain.Movement;
import pain.NativeScript;
import pain.Rotation;
import pain.GameObject;
import pain.Scriptable;

export namespace pain
{

class OrthoCameraEntity
    : public GameObject<MovementComponent, TransformComponent,
                        OrthoCameraComponent, RotationComponent,
                        NativeScriptComponent>
{
public:
  OrthoCameraEntity(Scene *scene, float aspectRatio, float zoomLevel);
};

class OrthoCameraController
    : public ExtendedScriptableEntity<MovementComponent, TransformComponent,
                                      OrthoCameraComponent, RotationComponent,
                                      NativeScriptComponent>
{
public:
  void onUpdate(double deltaTimeSec);
  void onRender(double currentTime) {};
  void onEvent(const SDL_Event &e);
  void recalculatePosition(const glm::vec3 &position, float rotation);

private:
  float m_zoomSpeed = 0.25f;
  bool onMouseScrolled(const SDL_Event &e, OrthoCameraComponent &cc);
  bool onWindowResized(const SDL_Event &e, OrthoCameraComponent &cc);
};

} // namespace pain
