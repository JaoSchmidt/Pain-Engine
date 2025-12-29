#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace pain
{
struct Renderer2d;
namespace Systems
{
struct LuaScript : public System<ComponentManager>,
                   IOnUpdate,
                   IOnEvent,
                   IOnRender {
  using System::System;
  void onUpdate(DeltaTime deltaTime) override;
  void onEvent(const SDL_Event &e) override;
  void onRender(Renderer2d &renderer, bool isMinimized,
                DeltaTime currentTime) override;
};
} // namespace Systems

} // namespace pain
