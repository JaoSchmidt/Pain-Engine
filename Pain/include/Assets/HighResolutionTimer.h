/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
// HighResolutionTimer.h
#pragma once

#include "DeltaTime.h"
#include <chrono>

namespace pain
{

/**
 * @class HighResolutionTimer
 * @brief High-precision timer utility for measuring elapsed time and frame
 * deltas.
 *
 * Uses `std::chrono::high_resolution_clock` internally.
 * Supports:
 * - Measuring frame delta time using tick()
 * - Querying total elapsed time
 * - Sleeping for a given duration
 */
class HighResolutionTimer
{
public:
  /// @brief Constructs the timer and initializes the starting time.
  HighResolutionTimer()
      : m_startTime(std::chrono::high_resolution_clock::now()),
        m_lastTime(m_startTime) {};

  /**
   * @brief Returns elapsed time since the last tick() call and updates the
   * internal clock.
   *
   * Typically used once per frame to compute delta time.
   *
   * @return DeltaTime representing the elapsed duration.
   */
  DeltaTime tick()
  {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now - m_lastTime;
    m_lastTime = now;

    uint64_t nanoSeconds = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
    return DeltaTime(nanoSeconds);
  }

  /**
   * @brief Returns elapsed time since timer creation in nanoseconds.
   *
   * @return Elapsed time in nanoseconds.
   */
  uint64_t elapsedNanos() const
  {
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_startTime)
            .count());
  }

  /**
   * @brief Returns elapsed time since timer creation as a DeltaTime object.
   *
   * @return Elapsed time.
   */
  DeltaTime elapsed() const { return DeltaTime(elapsedNanos()); }

  /**
   * @brief Returns the current system time in nanoseconds.
   *
   * Equivalent to a high-resolution performance counter.
   *
   * @return Current time in nanoseconds since epoch.
   */
  static uint64_t now()
  {
    auto timePoint = std::chrono::high_resolution_clock::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            timePoint.time_since_epoch())
            .count());
  }

  /**
   * @brief Suspends execution for a given number of milliseconds.
   *
   * @param milliseconds Duration to sleep.
   */
  static void sleep(uint32_t milliseconds)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
  }

  /**
   * @brief Suspends execution for a given DeltaTime duration.
   *
   * @param duration Duration to sleep.
   */
  static void sleep(const DeltaTime &duration)
  {
    std::this_thread::sleep_for(std::chrono::nanoseconds(duration.m_time));
  }

private:
  /** Time point when the timer was created. */
  std::chrono::high_resolution_clock::time_point m_startTime;
  /** Time point of the previous tick(). */
  std::chrono::high_resolution_clock::time_point m_lastTime;
};

} // namespace pain
