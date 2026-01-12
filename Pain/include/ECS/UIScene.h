#pragma once

#include "GUI/ImGuiComponent.h"
#include "Scene.h"
namespace pain
{

class UIScene : public AbstractScene<UIComponents>
{
public:
  using AbstractScene<UIComponents>::AbstractScene;

  static UIScene create(reg::EventDispatcher &eventDispatcher,
                        sol::state &solState, ThreadPool &threadPool);

  // =============================================================== //
  // IMGUI NATIVE SCRIPTING RELATED
  // =============================================================== //

  template <typename S>
    requires(UIComponents::isRegistered<ImGuiComponent>())
  S &getImGuiScript(reg::Entity entity)
  {
    ImGuiComponent &nsc = getComponent<ImGuiComponent>(entity);
    return static_cast<S &>(*nsc.instance);
  }

  template <typename N>
    requires(UIComponents::isRegistered<ImGuiComponent>())
  static N &emplaceImGuiScript(reg::Entity entity, UIScene &scene, N &&n)
  {
    ImGuiComponent &nsc = scene.getComponent<ImGuiComponent>(entity);
    nsc.bindAndInitiate<N>(std::move(n));
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<N &>(*nsc.instance);
  }

  // Emplace script inside the registry
  template <typename T, typename... Args>
    requires std::constructible_from<T, reg::Entity, UIScene &, Args...> &&
             (UIComponents::isRegistered<ImGuiComponent>())
  static T &emplaceImGuiScript(reg::Entity entity, UIScene &scene,
                               Args &&...args)
  {
    ImGuiComponent &nsc = scene.getComponent<ImGuiComponent>(entity);
    nsc.bindAndInitiate<T>(entity, scene, std::forward<Args>(args)...);
    if (nsc.instance && nsc.onCreateFunction)
      nsc.onCreateFunction(nsc.instance);
    return static_cast<T &>(*nsc.instance);
  }
};

} // namespace pain
