#pragma once

#include "ECS/Systems.h"

namespace pain
{
struct Renderer2d;
namespace Systems
{
struct Render : public System<ComponentManager> {
  using System::System;

  Render(Render &&other) noexcept : System(std::move(other))
  {
    std::cout << "Render move-constructed\n";
  }

  Render &operator=(Render &&other) noexcept
  {
    System::operator=(std::move(other));
    std::cout << "Render move-assigned\n";
    return *this;
  }
  void onRender(Renderer2d &renderer, bool isMinimized, double currentTime);
};
} // namespace Systems
} // namespace pain
