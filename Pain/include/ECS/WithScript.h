#pragma once
#include "ECS/Components/ComponentManager.h"
#include "ECS/Components/NativeScript.h"

#include "ECS/Registry/ArcheRegistry.h"
#include "ECS/Registry/Entity.h"
#include "GUI/ImGuiComponent.h"

namespace pain
{
namespace SceneHelper
{

template <typename T, typename... Args>
void withScript(reg::Entity entity,
                reg::ArcheRegistry<ComponentManager> &registry, Args &&...args)
{

  NativeScriptComponent &nsc =
      registry.getComponent<NativeScriptComponent>(entity);
  nsc.bindAndInitiate<T>(entity, std::forward<Args>(args)...);
  if (nsc.instance && nsc.onCreateFunction)
    nsc.onCreateFunction(nsc.instance);
}
template <typename T, typename... Args>
void withImGuiScript(reg::Entity entity,
                     reg::ArcheRegistry<ComponentManager> &registry,
                     Args &&...args)
{
  ImGuiComponent &nsc = registry.getComponent<ImGuiComponent>(entity);
  nsc.bindAndInitiate<T>(entity, std::forward<Args>(args)...);
  if (nsc.instance && nsc.onCreateFunction)
    nsc.onCreateFunction(nsc.instance);
}
} // namespace SceneHelper

} // namespace pain
