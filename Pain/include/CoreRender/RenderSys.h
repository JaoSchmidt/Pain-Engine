/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/** RenderSys.h */
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
/** ECS system responsible for submitting all visible 2D renderable components
 * to the renderer. */
struct Render : public System<WorldComponents>, IOnRender {
  /** Component signature required by this system. */
  using Tags = TypeList<Transform2dComponent, //
                        SpriteComponent,      //
                        RotationComponent>;
  using System::System;
  void onRender(Renderer2d &renderer, bool isMinimized,
                DeltaTime currentTime) override;
};
} // namespace Systems
} // namespace pain
