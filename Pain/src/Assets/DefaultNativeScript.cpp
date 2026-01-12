#include "Assets/DefaultNativeScript.h"

namespace pain
{

void resources::DefaultNativeScript::onUpdate(DeltaTime deltaTimeSec)
{
  UNUSED(deltaTimeSec)
  PLOG_W("You are updating a script that hasn't been initialized!");
}

void resources::DefaultNativeScript::onEvent(const SDL_Event &e)
{
  UNUSED(e)
  PLOG_W("You are updating a script that hasn't been initialized!");
}
void resources::DefaultNativeScript::onRender(Renderer2d &renderer,
                                              bool isMinimized,
                                              DeltaTime currentTime)
{
  UNUSED(renderer)
  UNUSED(isMinimized)
  UNUSED(currentTime)
  PLOG_W("You are trying to render a script that hasn't been initialized!");
}
void resources::DefaultNativeScript::onDestroy()
{
  PLOG_W("You are trying to destroy the default script, not sure if that is "
         "suppose to happen");
}

} // namespace pain
