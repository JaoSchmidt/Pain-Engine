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


#include "Assets/ManagerScripts.h"

namespace pain
{

void ScriptAssetManager::DefaultNativeScript::onUpdate(DeltaTime deltaTimeSec)
{
  UNUSED(deltaTimeSec)
  PLOG_W("You are updating a script that hasn't been initialized!");
}

void ScriptAssetManager::DefaultNativeScript::onEvent(const SDL_Event &e)
{
  UNUSED(e)
  PLOG_W("You are updating a script that hasn't been initialized!");
}
void ScriptAssetManager::DefaultNativeScript::onRender(Renderer2d &renderer,
                                                       bool isMinimized,
                                                       DeltaTime currentTime)
{
  UNUSED(renderer)
  UNUSED(isMinimized)
  UNUSED(currentTime)
  PLOG_W("You are trying to render a script that hasn't been initialized!");
}
void ScriptAssetManager::DefaultNativeScript::onDestroy()
{
  PLOG_W("You are trying to destroy the default script, not sure if that is "
         "suppose to happen");
}

} // namespace pain
