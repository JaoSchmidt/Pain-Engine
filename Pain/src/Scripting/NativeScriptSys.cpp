#include "Scripting/NativeScriptSys.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/NativeScript.h"

namespace pain
{
namespace Systems
{

void NativeScript::onUpdate(DeltaTime deltaTime)
{
  PROFILE_SCOPE("Scene::updateSystems - native scripts");

  auto chunks = query<NativeScriptComponent>();

  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      auto &nsc = scripts[i];

      if (nsc.instance && nsc.onUpdateFunction) {
        nsc.onUpdateFunction(nsc.instance, deltaTime);
      }
    }
  }
}

void NativeScript::onEvent(const SDL_Event &e)
{
  auto chunks = query<NativeScriptComponent>();

  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      NativeScriptComponent &nsc = scripts[i];

      if (nsc.instance && nsc.onEventFunction) {
        nsc.onEventFunction(nsc.instance, e);
      }
    }
  }
}

void NativeScript::onRender(Renderer2d &renderer, bool isMinimized,
                            DeltaTime currentTime)
{
  PROFILE_SCOPE("Scene::renderSystems - NativeScripts");

  auto chunks = query<NativeScriptComponent>();

  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      auto &nsc = scripts[i];

      if (nsc.instance && nsc.onRenderFunction) {
        nsc.onRenderFunction(nsc.instance, renderer, isMinimized, currentTime);
      }
    }
  }
}

} // namespace Systems
} // namespace pain
