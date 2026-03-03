/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// RenderSys.cpp
#include "CoreRender/RenderSys.h"
#include "CoreRender/MaterialComponent.h"
#include "CoreRender/MeshComponent.h"
#include "CoreRender/Renderer/RenderContext.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/Sprite.h"
#include "Physics/Movement3dComponent.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"

namespace pain
{
namespace Systems
{

constexpr std::array<SphereDivision, 3> sphereDivisions = {
    SphereDivision::D_8x8, SphereDivision::D_16x16, SphereDivision::D_32x32};

// =============================================================== //
// Render Components
// =============================================================== //
void Render::onRender(Renderers &renderer, bool isMinimized,
                      DeltaTime currentTime)
{
  UNUSED(isMinimized)
  UNUSED(currentTime)
  PROFILE_FUNCTION();
  {
    PROFILE_SCOPE("Scene::renderSystems - rotation quads");

    auto chunks =
        queryConst<Transform2dComponent, SpriteComponent, RotationComponent>();
    for (auto &chunk : chunks) {
      auto *t = std::get<0>(chunk.arrays);
      auto *s = std::get<1>(chunk.arrays);
      auto *r = std::get<2>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        std::visit(
            [&](auto &&tex) {
              using T = std::decay_t<decltype(tex)>;
              if constexpr (std::is_same_v<T, SheetStruct>) {
                renderer.renderer2d.drawQuad(
                    t[i].m_position, s[i].m_size, s[i].color,
                    r[i].m_rotationAngle, s[i].layer,
                    s[i].getTextureFromTextureSheet(), s[i].m_tilingFactor,
                    s[i].getCoords());
              } else {
                renderer.renderer2d.drawQuad(t[i].m_position, s[i].m_size,
                                             s[i].color, r[i].m_rotationAngle,
                                             s[i].layer, s[i].getTexture(),
                                             s[i].m_tilingFactor);
              }
            },
            s[i].m_tex);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - texture quads");
    auto chunks = queryConst<Transform2dComponent, SpriteComponent>(
        exclude<RotationComponent>);
    for (auto &chunk : chunks) {
      auto *t = std::get<0>(chunk.arrays);
      auto *s = std::get<1>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        std::visit(
            [&](auto &tex) {
              using T = std::decay_t<decltype(tex)>;
              if constexpr (std::is_same_v<T, SheetStruct>) {
                renderer.renderer2d.drawQuad(
                    t[i].m_position, s[i].m_size, s[i].color, s[i].layer,
                    s[i].getTextureFromTextureSheet(), s[i].m_tilingFactor,
                    s[i].getCoords());
              } else {
                renderer.renderer2d.drawQuad(
                    t[i].m_position, s[i].m_size, s[i].color, s[i].layer,
                    s[i].getTexture(), s[i].m_tilingFactor);
              }
            },
            s[i].m_tex);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - spriteless quads");
    auto chunks = queryConst<Transform2dComponent, SpritelessComponent>();
    for (auto &chunk : chunks) {
      auto *t = std::get<0>(chunk.arrays);
      auto *s = std::get<1>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        std::visit(
            [&](auto &&shape1) {
              using T1 = std::decay_t<decltype(shape1)>;
              if constexpr (std::is_same_v<T1, QuadShape>) {
                renderer.renderer2d.drawQuad(
                    t[i].m_position, shape1.size, s[i].color, s[i].layer,
                    TextureManager::getDefaultTexture(
                        TextureManager::DefaultTexture::Blank, false));
              } else if constexpr (std::is_same_v<T1, CircleShape>) {
                renderer.renderer2d.drawCircle(t[i].m_position, shape1.radius,
                                               s[i].color);
              }
            },
            s[i].m_shape);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - triangles");
    auto chunks = queryConst<Transform2dComponent, TrianguleComponent>();
    for (auto &chunk : chunks) {
      auto *t = std::get<0>(chunk.arrays);
      auto *tri = std::get<1>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        renderer.renderer2d.drawTri(t[i].m_position, tri[i].m_height,
                                    tri[i].m_color);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - spheres");
    auto chunks =
        query<Transform3dComponent, MeshComponent, MaterialComponent>();
    for (auto &chunk : chunks) {
      const auto *t = std::get<0>(chunk.arrays);
      const auto *mesh = std::get<1>(chunk.arrays);
      const auto *mat = std::get<2>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        if (mesh->shape == MeshShape::Cube) {
          // renderer.renderer3d.submitCube(t->m_position, mesh->size,
          //                                *mat->m_material);
        } else {
          renderer.renderer3d.submitUVSphere(t->m_position, mesh->size,
                                             sphereDivisions[mesh->shape],
                                             *mat->m_material);
        }
      }
    }
  }
}

} // namespace Systems
} // namespace pain
