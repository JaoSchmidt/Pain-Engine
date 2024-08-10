#include <pain.h>

class CameraController : public pain::ScriptableEntity
{
public:
  void onEvent(const SDL_Event &e);
  void onUpdate(double deltaTimeSe);

private:
  void onWindowResized(const SDL_Event &e, pain::OrthoCameraComponent &cc);
};
