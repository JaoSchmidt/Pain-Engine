#include "Scripting/NativeSystem.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/NativeScript.h"

namespace pain
{
namespace Systems
{

void NativeScript::onUpdate(double deltaTime)
{
  PROFILE_SCOPE("Scene::updateSystems - native scripts");
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    auto &nsc = *it;
    // if (!nsc.instance) {
    //   nsc.instantiateFunction(nsc.instance);
    //   nsc.instance->m_scene = this;
    //   nsc.instance->m_entity = it.getEntity();
    //
    //   if (nsc.onCreateFunction)
    //     nsc.onCreateFunction(nsc.instance);
    // }

    if (nsc.instance && nsc.onUpdateFunction)
      nsc.onUpdateFunction(nsc.instance, deltaTime);
  }
}
void NativeScript::onEvent(const SDL_Event &e)
{
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    auto &nsc = *it;
    if (nsc.instance && nsc.onEventFunction)
      nsc.onEventFunction(nsc.instance, e);
  }
}
void NativeScript::onRender(Renderer2d &renderer, bool isMinimized,
                            double currentTime)
{
  PROFILE_SCOPE("Scene::renderSystems - NativeScripts");
  for (auto it = begin<NativeScriptComponent>();
       it != end<NativeScriptComponent>(); ++it) {
    auto &nsc = *it;

    if (nsc.instance && nsc.onRenderFunction)
      nsc.onRenderFunction(nsc.instance, renderer, isMinimized, currentTime);
  }
}
} // namespace Systems
} // namespace pain
