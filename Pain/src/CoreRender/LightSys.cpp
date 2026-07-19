
/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// RenderSys.cpp
#include "CoreRender/LightSys.h"
#include "CoreRender/LightComponent.h"
#include "CoreRender/Renderer/RenderApi.h"
#include "Debugging/Profiling.h"
#include "Physics/Movement3dComponent.h"

namespace pain
{
namespace Systems
{

// =============================================================== //
// Render Components
// =============================================================== //
void LightSys::onRender(RenderApi &renderer, bool isMinimized,
                        DeltaTime currentTime)
{
  UNUSED(isMinimized)
  UNUSED(currentTime)
  PROFILE_FUNCTION();
  {
    PROFILE_SCOPE("Scene::renderLights");

    auto chunks = queryConst<Transform3dComponent, LightComponent>();
    for (auto &chunk : chunks) {
      const auto *t = std::get<0>(chunk.arrays);
      const auto *l = std::get<1>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        renderer.m_renderer3d.submitLight(t[i].m_position, l[i].m_color);
      }
    }
  }
}

} // namespace Systems
} // namespace pain
