#pragma once

#include "Assets/DeltaTime.h"
#include "Core.h"
#include "ECS/Components/ComponentManager.h"
#include "ECS/Systems.h"

namespace pain
{
// forward declare
struct Renderer2d;
struct Transform2dComponent;
struct RotationComponent;
struct SpriteComponent;

namespace Systems
{
struct Render : public System<WorldComponents>, IOnRender {
  using Tags = TypeList<Transform2dComponent, //
                        SpriteComponent,      //
                        RotationComponent>;
  using System::System;
  void onRender(Renderer2d &renderer, bool isMinimized,
                DeltaTime currentTime) override;
};
} // namespace Systems
} // namespace pain
