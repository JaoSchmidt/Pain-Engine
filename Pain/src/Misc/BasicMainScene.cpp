
#include "Misc/BasicMainScene.h"
#include "pain.h"

namespace pain
{

void BasicScene::syncSystems(Scene &scene)
{
  // Individually add each system, they will be executed inside the loop in this
  // order
  scene.addSystem<Systems::SweepAndPruneSys>();
  scene.addSystem<Systems::LightSys>();
  scene.addSystem<Systems::ParticleSys>();
  scene.addSystem<Systems::NativeScript>();
  scene.addSystem<Systems::LuaScript>();
  scene.addSystem<Systems::Kinematics>();
  scene.addSystem<Systems::LuaSchedulerSys>();
  scene.addSystem<Systems::Render2d>();
  scene.addSystem<Systems::Render3d>();
}

} // namespace pain
