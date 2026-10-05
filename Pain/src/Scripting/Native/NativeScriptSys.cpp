/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Scripting/Native/NativeScriptSys.h"
#include "CoreRender/Renderer/RenderApi.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/NativeScript.h"

namespace pain
{
namespace Systems
{
namespace detail
{

void _implOnUpdate(DeltaTime deltaTime,
                   std::vector<reg::ChunkView<NativeScriptComponent>> chunks)
{
  PROFILE_SCOPE("Scene::updateSystems - native scripts");

  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      auto &nsc = scripts[i];

      if (nsc.instance && nsc.onUpdateFunction) {
        nsc.onUpdateFunction(nsc.instance.get(), deltaTime);
      }
    }
  }
}

void _implOnEvent(const SDL_Event &e,
                  std::vector<reg::ChunkView<NativeScriptComponent>> chunks)
{
  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      NativeScriptComponent &nsc = scripts[i];

      if (nsc.instance && nsc.onEventFunction) {
        nsc.onEventFunction(nsc.instance.get(), e);
      }
    }
  }
}

void _implOnRender(RenderApi &renderAPI, DeltaTime currentTime,
                   std::vector<reg::ChunkView<NativeScriptComponent>> chunks)
{
  UNUSED(renderAPI);
  PROFILE_SCOPE("Scene::renderSystems - NativeScripts");

  for (auto &chunk : chunks) {
    auto *scripts = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      auto &nsc = scripts[i];

      if (nsc.instance && nsc.onRenderFunction) {
        nsc.onRenderFunction(nsc.instance.get(), renderAPI, currentTime);
      }
    }
  }
}

} // namespace detail
} // namespace Systems
} // namespace pain
