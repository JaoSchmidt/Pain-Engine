#pragma once

#include "pch.h"

#include "CoreFiles/DeltaTime.h"
#include "ECS/Entity.h"
#include <sys/types.h>

// OBS: I'm not using this atm

namespace pain
{
using Tag = uint32_t; // max 32 flags
class CompTags
{
  static constexpr Tag Position = 0x1 << 0;
  static constexpr Tag Velocity = 0x1 << 1;
  static constexpr Tag Acceleration = 0x1 << 2;
  static constexpr Tag Renderable = 0x1 << 3;
  static constexpr Tag Health = 0x1 << 4;
};

} // namespace pain
