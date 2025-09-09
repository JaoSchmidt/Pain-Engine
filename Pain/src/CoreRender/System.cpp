#include "CoreRender/System.h"
#include "CoreRender/Renderer/Renderer2d.h"
#include "Debugging/Profiling.h"

namespace pain
{
namespace Systems
{

// =============================================================== //
// Render Components
// =============================================================== //
void Render::onRender(bool isMinimized, double currentTime)
{
  PROFILE_FUNCTION();
  {
    PROFILE_SCOPE("Scene::renderSystems - texture quads");
    auto [tIt, sIt] = begin<TransformComponent, SpriteComponent>();
    const auto &[tItEnd, sItEnd] = end<TransformComponent, SpriteComponent>();

    for (; tIt != tItEnd; ++tIt, ++sIt) {
      const TransformComponent &tc = *tIt;
      const SpriteComponent &sc = *sIt;
      Renderer2d::drawQuad(tc.m_position, sc.m_size, sc.m_color,
                           sc.getTexture(), sc.m_tilingFactor);
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - rotation quads");
    auto [tIt, sIt, rIt] =
        begin<TransformComponent, SpriteComponent, RotationComponent>();
    const auto &[tItEnd, sItEnd2, rItEnd] =
        end<TransformComponent, SpriteComponent, RotationComponent>();

    for (; tIt != tItEnd; ++tIt, ++rIt, ++sIt) {
      Renderer2d::drawQuad(tIt->m_position, sIt->m_size, sIt->m_color,
                           rIt->m_rotationAngle, sIt->getTexture(),
                           sIt->m_tilingFactor);
      // TODO: Remove m_rotation of rc... should only
      // have angle, in the case of the camera
      // inclune rot direction in its script
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - spriteless quads");
    auto [tIt, sIt] = begin<TransformComponent, SpritelessComponent>();
    auto [tItEnd, sItEnd] = end<TransformComponent, SpritelessComponent>();
    for (; tIt != tItEnd; ++tIt, ++sIt) {
      Renderer2d::drawQuad(tIt->m_position, sIt->m_size, sIt->m_color,
                           resources::getDefaultTexture(resources::BLANK));
    }
  }
  {
    PROFILE_SCOPE("Scene::renderSystems - triangles");
    auto [tIt, triIt] = begin<TransformComponent, TrianguleComponent>();
    auto [tItEnd, triItEnd] = end<TransformComponent, TrianguleComponent>();
    for (; tIt != tItEnd; ++tIt, ++triIt) {
      Renderer2d::drawTri(tIt->m_position, triIt->m_height, triIt->m_color);
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
      Renderer2d::beginSprayParticle((float)currentTime, psc);
      for (Particle &pa : psc.m_particles) {
        if (pa.m_alive)
          Renderer2d::drawSprayParticle(pa);
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
