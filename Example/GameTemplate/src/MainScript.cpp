#include "MainScript.h"
#include <pain.h>
#include <painless.h>

MainScript &MainScript::createScriptScene(pain::Scene &scene,
                                          pain::Application *app)
{
  pain::RenderApi &renderAPI = app->getRenderApi();
  return pain::Scene::emplaceScript<MainScript>(scene.getEntity(), scene);
}
MainScript::MainScript(reg::Entity entity, pain::Scene &scene)
    : pain::WorldObject(entity, scene) {};
