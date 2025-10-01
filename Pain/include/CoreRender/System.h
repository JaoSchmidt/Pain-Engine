#pragma once

#include "CoreRender/Renderer/Renderer2d.h"
#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct Render : public System {
  using System::System;
  ~Render() { std::cout << "Render destructed\n"; }

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
