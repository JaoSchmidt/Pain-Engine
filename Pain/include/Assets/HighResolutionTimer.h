/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once

#include "DeltaTime.h"
#include <chrono>

namespace pain
{

class HighResolutionTimer
{
public:
  HighResolutionTimer()
      : m_startTime(std::chrono::high_resolution_clock::now()),
        m_lastTime(m_startTime)
  {
  }

  // Get elapsed time since last tick() call and advance internal clock
  DeltaTime tick()
  {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now - m_lastTime;
    m_lastTime = now;

    uint64_t nanoSeconds = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
    return DeltaTime(nanoSeconds);
  }

  // Get absolute elapsed time since timer creation (in nanoseconds)
  uint64_t elapsedNanos() const
  {
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_startTime)
            .count());
  }

  // Get absolute elapsed time since timer creation
  DeltaTime elapsed() const { return DeltaTime(elapsedNanos()); }

  // Get current time as nanoseconds (equivalent to SDL_GetPerformanceCounter)
  // Returns absolute system time
  static uint64_t now()
  {
    auto timePoint = std::chrono::high_resolution_clock::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            timePoint.time_since_epoch())
            .count());
  }

  // Sleep for milliseconds (equivalent to SDL_Delay)
  static void sleep(uint32_t milliseconds)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  }

  // Sleep for duration
  static void sleep(const DeltaTime &duration)
  {
    std::this_thread::sleep_for(std::chrono::nanoseconds(duration.m_time));
  }

private:
  std::chrono::high_resolution_clock::time_point m_startTime;
  std::chrono::high_resolution_clock::time_point m_lastTime;
};

} // namespace pain
