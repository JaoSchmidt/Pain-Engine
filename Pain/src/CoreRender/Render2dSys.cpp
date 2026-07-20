/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// RenderSys.cpp
#include "CoreRender/Render2dSys.h"
#include "CoreRender/ColorIndexComponent.h"
#include "CoreRender/MaterialComponent.h"
#include "CoreRender/RenderContext.h"
#include "CoreRender/Renderer/RenderApi.h"
#include "CoreRender/SpriteComponent.h"
#include "Debugging/Profiling.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"
#include "TextComponent.h"

namespace pain
{
namespace Systems
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
void Render2d::onRender(RenderApi &renderer, 
                        DeltaTime currentTime)
{
  
  UNUSED(currentTime)
  PROFILE_FUNCTION();
  Renderer2d &renderer2d = renderer.m_renderer2d;
  {
    PROFILE_SCOPE("Scene::renderSystems - rotation quads");

    auto chunks =
        queryConst<Transform2dComponent, SpriteComponent, RotationComponent,
                   MaterialComponent>(exclude<ColorIndexComponent>);
    for (auto &chunk : chunks) {
      const auto *t = std::get<0>(chunk.arrays);
      const auto *s = std::get<1>(chunk.arrays);
      const auto *r = std::get<2>(chunk.arrays);
      const MaterialComponent *m = std::get<3>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        std::visit(
            [&](auto &shape) {
              using T = std::decay_t<decltype(shape)>;
              if constexpr (std::is_same_v<T, QuadShape>) {
                renderer2d.submitQuad(t[i].m_position, shape.side,
                                      r[i].m_rotationRadians, s[i].layer,
                                      *m[i]);
              } else if constexpr (std::is_same_v<T, RectShape>) {
                renderer2d.submitRect(t[i].m_position, shape.size,
                                      r[i].m_rotationRadians, s[i].layer,
                                      *m[i]);
              } else if constexpr (std::is_same_v<T, TriangleShape>) {
                renderer2d.submitTri(t[i].m_position,
                                     {shape.base, shape.height},
                                     r[i].m_rotationRadians, s[i].layer, *m[i]);
              } else if constexpr (std::is_same_v<T, LineShape>) {
                renderer2d.submitLine(t[i].m_position, shape.destination,
                                      shape.thickness, s[i].layer, *m[i]);
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
            exclude<RotationComponent, ColorIndexComponent>);
    for (auto &chunk : chunks) {
      const auto *t = std::get<0>(chunk.arrays);
      const auto *s = std::get<1>(chunk.arrays);
      const MaterialComponent *m = std::get<2>(chunk.arrays);
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
              } else if constexpr (std::is_same_v<T, LineShape>) {
                renderer2d.submitLine(t[i].m_position, shape.destination,
                                      shape.thickness, s[i].layer, *m[i]);
              }
            },
            s[i].m_shape);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - colored primitives");
    auto chunks =
        queryConst<Transform2dComponent, SpriteComponent, ColorIndexComponent,
                   MaterialComponent>(exclude<RotationComponent>);
    for (auto &chunk : chunks) {
      const auto *t = std::get<0>(chunk.arrays);
      const auto *s = std::get<1>(chunk.arrays);
      const auto *c = std::get<2>(chunk.arrays);
      const auto *m = std::get<3>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        std::visit(
            [&](auto &shape) {
              using T = std::decay_t<decltype(shape)>;
              if constexpr (std::is_same_v<T, QuadShape>) {
                renderer2d.submitQuad(t[i].m_position, shape.side, s[i].layer,
                                      *m[i], c[i].color);
              } else if constexpr (std::is_same_v<T, RectShape>) {
                renderer2d.submitRect(t[i].m_position, shape.size, s[i].layer,
                                      *m[i], c[i].color);
              } else if constexpr (std::is_same_v<T, LineShape>) {
                renderer2d.submitLine(t[i].m_position, shape.destination,
                                      shape.thickness, s[i].layer, *m[i],
                                      c[i].color);
              }
            },
            s[i].m_shape);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - colored primitives");
    auto chunks = queryConst<Transform2dComponent, TextComponent>(
        exclude<RotationComponent>);
    for (auto &chunk : chunks) {
      const auto *p = std::get<0>(chunk.arrays);
      const auto *t = std::get<1>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        renderer2d.submitString(p[i].m_position, t[i].scale, t[i].text,
                                t[i].font, t[i].color, t[i].align);
      }
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - scripts");
    const auto &commands = renderer.m_renderContext.getCommands();
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
      case RenderCommandType::Text:
        renderer2d.submitString(      //
            cmd.m_data.text.position, //
            cmd.m_data.text.scale,    //
            cmd.m_data.text.string,   //
            *cmd.m_data.text.font,    //
            cmd.m_data.text.color);
        break;
      case RenderCommandType::Rect:
        renderer2d.submitRect(           //
            cmd.m_data.sprite.transform, //
            cmd.m_data.sprite.layer,     //
            *cmd.m_data.sprite.material);
        break;
      case RenderCommandType::Line:
        renderer2d.submitLine(           //
            cmd.m_data.line.origin,      //
            cmd.m_data.line.destination, //
            cmd.m_data.line.thickness,   //
            cmd.m_data.line.layer,       //
            *cmd.m_data.sprite.material);
        break;

      default:
        break;
      }
    }
  }
}

} // namespace Systems
} // namespace pain
