#include "CoreRender/RenderSys.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/Movement.h"
#include "ECS/Components/Rotation.h"
#include "ECS/Components/Sprite.h"

namespace pain
{
namespace Systems
{

// =============================================================== //
// Render Components
// =============================================================== //
void Render::onRender(Renderer2d &renderer, UNUSED bool isMinimized,
                      DeltaTime currentTime)
{
  PROFILE_FUNCTION();
  {
    PROFILE_SCOPE("Scene::renderSystems - texture quads");
    auto [tIt, sIt] = begin<Transform2dComponent, SpriteComponent>();
    const auto &[tItEnd, sItEnd] = end<Transform2dComponent, SpriteComponent>();
    for (; tIt != tItEnd; ++tIt, ++sIt) {
      std::visit(
          [&](auto &&tex) {
            using T = std::decay_t<decltype(tex)>;
            if constexpr (std::is_same_v<T, SheetStruct>) {
              renderer.drawQuad(tIt->m_position, sIt->m_size, sIt->m_color,
                                sIt->getTextureFromTextureSheet(),
                                sIt->m_tilingFactor, sIt->getCoords());
            } else {
              renderer.drawQuad(tIt->m_position, sIt->m_size, sIt->m_color,
                                sIt->getTexture(), sIt->m_tilingFactor);
            }
          },
          sIt->m_tex);
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - rotation quads");
    auto [tIt, sIt, rIt] =
        begin<Transform2dComponent, SpriteComponent, RotationComponent>();
    const auto &[tItEnd, sItEnd2, rItEnd] =
        end<Transform2dComponent, SpriteComponent, RotationComponent>();

    for (; tIt != tItEnd; ++tIt, ++rIt, ++sIt) {
      // TODO: Remove m_rotation of rc... should only
      // have angle, in the case of the camera
      // inclune rot direction in its script
      std::visit(
          [&](auto &&tex) {
            using T = std::decay_t<decltype(tex)>;
            if constexpr (std::is_same_v<T, SheetStruct>) {
              renderer.drawQuad(tIt->m_position, sIt->m_size, sIt->m_color,
                                sIt->getTextureFromTextureSheet(),
                                sIt->m_tilingFactor, sIt->getCoords());
            } else {
              renderer.drawQuad(tIt->m_position, sIt->m_size, sIt->m_color,
                                sIt->getTexture(), sIt->m_tilingFactor);
            }
          },
          sIt->m_tex);

      // renderer.drawQuad(tIt->m_position, sIt->m_size, sIt->m_color,
      //                   rIt->m_rotationAngle, sIt->getTexture(),
      //                   sIt->m_tilingFactor);
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - spriteless quads");
    auto [tIt, sIt] = begin<Transform2dComponent, SpritelessComponent>();
    auto [tItEnd, sItEnd] = end<Transform2dComponent, SpritelessComponent>();
    for (; tIt != tItEnd; ++tIt, ++sIt) {
      std::visit(
          [&](auto &&shape1) {
            using T1 = std::decay_t<decltype(shape1)>;
            if constexpr (std::is_same_v<T1, QuadShape>) {
              renderer.drawQuad(tIt->m_position, shape1.size, sIt->m_color,
                                resources::getDefaultTexture(
                                    resources::DefaultTexture::Blank, false));
            } else if constexpr (std::is_same_v<T1, CircleShape>) {
              renderer.drawCircle(tIt->m_position, shape1.radius, sIt->m_color);
            }
          },
          sIt->m_shape);
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - triangles");
    auto [tIt, triIt] = begin<Transform2dComponent, TrianguleComponent>();
    auto [tItEnd, triItEnd] = end<Transform2dComponent, TrianguleComponent>();
    for (; tIt != tItEnd; ++tIt, ++triIt) {
      renderer.drawTri(tIt->m_position, triIt->m_height, triIt->m_color);
    }
  }
  // =============================================================== //
  // Render Particle Systems
  // =============================================================== //
  {
    PROFILE_SCOPE("Scene::renderSystems - Particles");
    for (auto it = begin<ParticleSprayComponent>();
         it != end<ParticleSprayComponent>(); ++it) {
      ParticleSprayComponent &psc = *it;
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

} // namespace Systems
} // namespace pain
