#pragma once
#include "ECS/Scriptable.h"
namespace pain
{

namespace resources
{

class DefaultNativeScript : public WorldObject
{
  void onUpdate(DeltaTime deltaTimeSec);
  void onEvent(const SDL_Event &e);
  void onRender(Renderer2d &renderer, bool isMinimized, DeltaTime currentTime);
  void onDestroy();
};

} // namespace resources
} // namespace pain
