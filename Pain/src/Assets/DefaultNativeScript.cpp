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
