#pragma once
#include "ECS/Components/ComponentManager.h"
#include "ECS/Components/NativeScript.h"
#include "ECS/Registry/ArcheRegistry.h"
#include "Entity.h"
#include "GUI/ImGuiComponent.h"

namespace pain
{
namespace SceneHelper
{

template <typename T, typename... Args>
void withScript(Entity entity, ArcheRegistry &registry, Args &&...args)
{
  // NOTE: yes I'm assuming that Scene won't have anything besides a script
  // component if that's not the case, then just think of something besides
  Bitmask bitmask =
      ComponentManager::singleComponentBitmask<NativeScriptComponent>();

  NativeScriptComponent &nsc =
      registry.getComponent<NativeScriptComponent>(entity, bitmask);
  nsc.bindAndInitiate<T>(entity, bitmask, std::forward<Args>(args)...);
  if (nsc.instance && nsc.onCreateFunction)
    nsc.onCreateFunction(nsc.instance);
}
template <typename T, typename... Args>
void withImGuiScript(Entity entity, ArcheRegistry &registry, Args &&...args)
{
  Bitmask bitmask =
      ComponentManager::singleComponentBitmask<NativeScriptComponent>();
  ImGuiComponent &nsc = registry.getComponent<ImGuiComponent>(entity, bitmask);
  nsc.bindAndInitiate<T>(entity, bitmask, std::forward<Args>(args)...);
  if (nsc.instance && nsc.onCreateFunction)
    nsc.onCreateFunction(nsc.instance);
}
} // namespace SceneHelper

} // namespace pain
