#include <pain.h>

class CameraController : public pain::ScriptableEntity
{
public:
  void onEvent(const SDL_Event &e);

private:
  void onWindowResized(const SDL_Event &e, pain::OrthoCameraComponent &cc);
};
