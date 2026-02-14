/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
#include "ECS/Scriptable.h"
namespace pain
{

namespace ScriptAssetManager
{

class DefaultNativeScript : public WorldObject
{
  void onUpdate(DeltaTime deltaTimeSec);
  void onEvent(const SDL_Event &e);
  void onRender(Renderers &renderer, bool isMinimized, DeltaTime currentTime);
  void onDestroy();
};

} // namespace ScriptAssetManager
} // namespace pain
