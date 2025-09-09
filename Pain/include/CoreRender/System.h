#pragma once

#include "ECS/Systems.h"

namespace pain
{
namespace Systems
{
struct Render : public System {
  using System::System;
  void onRender(bool isMinimized, double currentTime);
};
} // namespace Systems
} // namespace pain
