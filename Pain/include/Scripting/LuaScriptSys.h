#pragma once
#include "Assets/DeltaTime.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace pain
{
struct Renderer2d;
struct LuaScriptComponent;

namespace Systems
{
struct LuaScript : public System<WorldComponents>,
                   IOnUpdate,
                   IOnEvent,
                   IOnRender {
  using Tags = TypeList<LuaScriptComponent>;
  using System::System;
  void onUpdate(DeltaTime deltaTime) override;
  void onEvent(const SDL_Event &e) override;
  void onRender(Renderer2d &renderer, bool isMinimized,
                DeltaTime currentTime) override;
};
} // namespace Systems

} // namespace pain
