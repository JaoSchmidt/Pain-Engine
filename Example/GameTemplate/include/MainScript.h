#pragma once
#include <pain.h>
#include <painless.h>

class MainScript : public pain::WorldObject
{

public:
  static MainScript &createScriptScene(pain::Scene &scene,
                                       pain::Application *app);
  MainScript(reg::Entity entity, pain::Scene &scene);

  /** game script functions
    void onRender(pain::RenderContext &rs, bool isMinimazed,
                  pain::DeltaTime currentTime) {};
    void onUpdate(pain::DeltaTime dt) {};
    void onCreate() {};
    void onEvent(const SDL_Event &e) {};
  */
};
