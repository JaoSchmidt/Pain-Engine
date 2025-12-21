#include "Misc/CameraSys.h"
#include "Debugging/Profiling.h"
#include "ECS/Components/Camera.h"
namespace pain
{

namespace Systems
{
void onWindowResized(const SDL_Event &event, OrthoCameraComponent &cc)
{
  cc.m_aspectRatio = (float)event.window.data1 / (float)event.window.data2;
  cc.m_matrices->SetProjection(-cc.m_aspectRatio * cc.m_zoomLevel,
                               cc.m_aspectRatio * cc.m_zoomLevel,
                               -cc.m_zoomLevel, cc.m_zoomLevel);
  cc.m_matrices->setResolution(event.window.data1, event.window.data2);
}

void Systems::CameraSys::onEvent(const SDL_Event &event)
{
  PROFILE_FUNCTION();

  // =============================================================== //
  // Update Resolution change
  // =============================================================== //

  auto chunks = query<OrthoCameraComponent>();

  for (auto &chunk : chunks) {
    auto *__restrict c = std::get<0>(chunk.arrays);

    for (size_t i = 0; i < chunk.count; ++i) {
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          onWindowResized(event, c[i]);
        }
      }
    }
  }
}

} // namespace Systems
} // namespace pain
