/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// RenderSys.cpp
#include "CoreRender/RenderSys.h"
#include "CoreRender/MaterialComponent.h"
#include "CoreRender/MeshComponent.h"
#include "CoreRender/RenderContext.h"
#include "CoreRender/Renderer/Renderers.h"
#include "CoreRender/SpriteComponent.h"
#include "Debugging/Profiling.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"

namespace pain
{
namespace Systems
{
// NOTE: pay attention that the systems are communicating directly with the
// renderers, meaning the state of the API (opengl, vulkan, directx) NEEDS to be
// programmed to render the batch at any moment, including the transparency,
// depth, dual face, etc
// This means you probl. need to call like backend::enableXX() from here

// constexpr std::array<SphereDivision, 3> sphereDivisions = {
//     SphereDivision::D_8x8, SphereDivision::D_16x16, SphereDivision::D_32x32};

// =============================================================== //
// Render Components
// =============================================================== //
void Render::onRender(Renderers &renderer, bool isMinimized,
                      DeltaTime currentTime)
{
  UNUSED(isMinimized)
  UNUSED(currentTime)
  PROFILE_FUNCTION();
  Renderer2d &renderer2d = renderer.m_renderer2d;
  {
    PROFILE_SCOPE("Scene::renderSystems - rotation quads");

    auto chunks = queryConst<Transform2dComponent, SpriteComponent,
                             RotationComponent, MaterialComponent>();
    for (auto &chunk : chunks) {
      auto *t = std::get<0>(chunk.arrays);
      auto *s = std::get<1>(chunk.arrays);
      auto *r = std::get<2>(chunk.arrays);
      const MaterialComponent *m = std::get<3>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        std::visit(
            [&](auto &shape) {
              using T = std::decay_t<decltype(shape)>;
              if constexpr (std::is_same_v<T, QuadShape>) {
                renderer2d.submitQuad(t[i].m_position, shape.side,
                                      r[i].m_rotationAngle, s[i].layer, *m[i]);
              } else if constexpr (std::is_same_v<T, RectShape>) {
                renderer2d.submitRect(t[i].m_position, shape.size,
                                      r[i].m_rotationAngle, s[i].layer, *m[i]);
              } else if constexpr (std::is_same_v<T, TriangleShape>) {
                renderer2d.submitTri(t[i].m_position,
                                     {shape.base, shape.height},
                                     r[i].m_rotationAngle, s[i].layer, *m[i]);
              } else if constexpr (std::is_same_v<T, RectShape>) {
                renderer2d.submitRect(t[i].m_position, shape.size,
                                      r[i].m_rotationAngle, s[i].layer, *m[i]);
              }
            },
            s[i].m_shape);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - texture quads");
    auto chunks =
        queryConst<Transform2dComponent, SpriteComponent, MaterialComponent>(
            exclude<RotationComponent>);
    for (auto &chunk : chunks) {
      auto *t = std::get<0>(chunk.arrays);
      auto *s = std::get<1>(chunk.arrays);
      auto *m = std::get<2>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        std::visit(
            [&](auto &shape) {
              using T = std::decay_t<decltype(shape)>;
              if constexpr (std::is_same_v<T, QuadShape>) {
                renderer2d.submitQuad(t[i].m_position, shape.side, s[i].layer,
                                      *m[i]);
              } else if constexpr (std::is_same_v<T, RectShape>) {
                renderer2d.submitRect(t[i].m_position, shape.size, s[i].layer,
                                      *m[i]);
              } else if constexpr (std::is_same_v<T, TriangleShape>) {
                renderer2d.submitTri(t[i].m_position,
                                     {shape.base, shape.height}, s[i].layer,
                                     *m[i]);
              }
            },
            s[i].m_shape);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - scripts");
    auto &commands = renderer.m_renderContext.getCommands();

    for (const auto &cmd : commands) {
      switch (cmd.m_type) {
      case RenderCommandType::Quad:
        renderer2d.submitQuad(           //
            cmd.m_data.sprite.transform, //
            cmd.m_data.sprite.layer,     //
            *cmd.m_data.sprite.material);
        break;
      case RenderCommandType::Triangle:
        renderer2d.submitTri(            //
            cmd.m_data.sprite.transform, //
            cmd.m_data.sprite.layer,     //
            *cmd.m_data.sprite.material);
        break;
      case RenderCommandType::Rect:
        renderer2d.submitRect(           //
            cmd.m_data.sprite.transform, //
            cmd.m_data.sprite.layer,     //
            *cmd.m_data.sprite.material);
        break;

      default:
        break;
      }
    }
  }

  Renderer3d &renderer3d = renderer.m_renderer3d;
  {
    PROFILE_SCOPE("Scene::renderSystems - spheres");
    auto chunks =
        query<Transform3dComponent, MeshComponent, MaterialComponent>();
    for (auto &chunk : chunks) {
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
    auto &commands = renderer.m_renderContext.getCommands();

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
  renderer.m_renderContext.clear();
}

} // namespace Systems
} // namespace pain
