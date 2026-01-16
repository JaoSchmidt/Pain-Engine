/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "CoreRender/RenderSys.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/Sprite.h"
#include "Physics/MovementComponent.h"
#include "Physics/RotationComponent.h"

namespace pain
{
namespace Systems
{

// =============================================================== //
// Render Components
// =============================================================== //
void Render::onRender(Renderer2d &renderer, bool isMinimized,
                      DeltaTime currentTime)
{
  UNUSED(isMinimized)
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
                renderer.drawQuad(t[i].m_position, s[i].m_size, s[i].color,
                                  r[i].m_rotationAngle, s[i].layer,
                                  s[i].getTextureFromTextureSheet(),
                                  s[i].m_tilingFactor, s[i].getCoords());
              } else {
                renderer.drawQuad(t[i].m_position, s[i].m_size, s[i].color,
                                  r[i].m_rotationAngle, s[i].layer,
                                  s[i].getTexture(), s[i].m_tilingFactor);
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
                renderer.drawQuad(t[i].m_position, s[i].m_size, s[i].color,
                                  s[i].layer, s[i].getTextureFromTextureSheet(),
                                  s[i].m_tilingFactor, s[i].getCoords());
              } else {
                renderer.drawQuad(t[i].m_position, s[i].m_size, s[i].color,
                                  s[i].layer, s[i].getTexture(),
                                  s[i].m_tilingFactor);
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
                renderer.drawQuad(
                    t[i].m_position, shape1.size, s[i].color, s[i].layer,
                    TextureManager::getDefaultTexture(
                        TextureManager::DefaultTexture::Blank, false));
              } else if constexpr (std::is_same_v<T1, CircleShape>) {
                renderer.drawCircle(t[i].m_position, shape1.radius, s[i].color);
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
        renderer.drawTri(t[i].m_position, tri[i].m_height, tri[i].m_color);
      }
    }
  }
  // =============================================================== //
  // Render Particle Systems
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::renderSystems - Particles");

    auto chunks = query<ParticleSprayComponent>();
    for (auto &chunk : chunks) {
      auto *p = std::get<0>(chunk.arrays);
      for (size_t i = 0; i < chunk.count; ++i) {
        ParticleSprayComponent &psc = p[i];
        renderer.beginSprayParticle(currentTime, psc);
        for (Particle &pa : psc.m_particles) {
          if (pa.m_alive)
            renderer.drawSprayParticle(pa);
          // Remove dead particles
          if (currentTime - pa.m_startTime >= psc.m_lifeTime) {
            pa.m_alive = false;
          }
        }
      }
    }
  }
}

} // namespace Systems
} // namespace pain
