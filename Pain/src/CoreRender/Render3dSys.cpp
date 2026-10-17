/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// RenderSys.cpp
#include "CoreRender/Render3dSys.h"
#include "CoreRender/MaterialComponent.h"
#include "CoreRender/MeshComponent.h"
#include "CoreRender/RenderContext.h"
#include "CoreRender/Renderer/RenderApi.h"
#include "Debugging/Profiling.h"
#include "Physics/Movement3dComponent.h"

namespace pain
{
namespace Systems::detail
{
// NOTE: pay attention that the systems are communicating directly with the
// renderAPI, meaning the state of the API (opengl, vulkan, directx) NEEDS to be
// programmed to render the batch at any moment, including the transparency,
// depth, dual face, etc
// This means you probl. need to call like backend::enableXX() from here

// constexpr std::array<SphereDivision, 3> sphereDivisions = {
//     SphereDivision::D_8x8, SphereDivision::D_16x16, SphereDivision::D_32x32};

// =============================================================== //
// Render Components
// =============================================================== //
void _implOnRender(
    RenderApi &renderer, DeltaTime currentTime,
    std::vector<
        reg::ChunkView<Transform3dComponent, MeshComponent, MaterialComponent>>
        chunksMesh)
{
  UNUSED(currentTime)
  PROFILE_FUNCTION();
  Renderer3d &renderer3d = renderer.m_renderer3d;
  {
    PROFILE_SCOPE("Scene::renderSystems - spheres");
    for (auto &chunk : chunksMesh) {
      const auto *t = std::get<0>(chunk.arrays);
      const auto *mesh = std::get<1>(chunk.arrays);
      const MaterialComponent *m = std::get<2>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        if (mesh->shape == MeshShape::Cube) {
          renderer3d.submitCube(t->m_position, mesh->size, *m[i]);
        } else {
          renderer3d.submitUVSphere(t->m_position, mesh->size,
                                    s_sphereDivisions[mesh->shape], *m[i]);
        }
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - scripts");
    const auto &commands = renderer.m_renderContext.getCommands();

    for (const auto &cmd : commands) {
      switch (cmd.m_type) {
      case RenderCommandType::Cube:
        renderer3d.submitCube(         //
            cmd.m_data.mesh.transform, //
            *cmd.m_data.mesh.material);
        break;
      case RenderCommandType::Sphere:
        renderer3d.submitUVSphere(       //
            cmd.m_data.sphere.transform, //
            cmd.m_data.sphere.div,       //
            *cmd.m_data.sphere.material);
        break;
      case RenderCommandType::LightPoint:
        renderer3d.submitLight(             //
            cmd.m_data.lightPoint.position, //
            cmd.m_data.lightPoint.color);
        break;

      default:
        break;
      }
    }
  }
}

} // namespace Systems::detail
} // namespace pain
