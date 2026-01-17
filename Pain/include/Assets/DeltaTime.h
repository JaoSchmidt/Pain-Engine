/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
// DeltaTime.h
#pragma once

#include "pch.h"

namespace pain
{

/**
 * @class DeltaTime
 * @brief Represents a duration stored internally in nanoseconds.
 *
 * Provides convenient conversion helpers and arithmetic operators
 * for time-based calculations.
 */
class DeltaTime
{
public:
  /**
   * @brief Constructs a DeltaTime value.
   *
   * @param nanoSeconds Duration in nanoseconds.
   */
  DeltaTime(uint64_t nanoSeconds = 0) : m_time(nanoSeconds) {}

  /** @return Duration in seconds (double precision). */
  inline double getSeconds() const
  {
    return static_cast<double>(m_time) * sec_d;
  }

  /** @return Duration in milliseconds (double precision). */
  inline double getMiliSeconds() const
  {
    return static_cast<double>(m_time) * milsec_d;
  }

  /** @return Duration in microseconds (double precision). */
  inline double getMicroSeconds() const
  {
    return static_cast<double>(m_time) * microsec_d;
  }

  /** @return Duration in nanoseconds (double precision). */
  inline double getNanoSeconds() const { return static_cast<double>(m_time); }

  /** @return Duration in seconds (float precision). */
  inline float getSecondsf() const
  {
    return static_cast<float>(m_time) * sec_f;
  }

  /** @return Duration in milliseconds (float precision). */
  inline float getMiliSecondsf() const
  {
    return static_cast<float>(m_time) * milsec_f;
  }

  /** @return Duration in microseconds (float precision). */
  inline float getMicroSecondsf() const
  {
    return static_cast<float>(m_time) * microsec_f;
  }

  /** @return Duration in nanoseconds (float precision). */
  inline float getNanoSecondsf() const { return static_cast<float>(m_time); }

  /** @brief Adds two DeltaTime values. */
  inline DeltaTime operator+(const DeltaTime &o) const
  {
    return DeltaTime(m_time + o.m_time);
  }

  /** @brief Adds another DeltaTime to this instance. */
  inline DeltaTime operator+=(const DeltaTime &o)
  {
    m_time += o.m_time;
    return *this;
  }

  /** @brief Subtracts two DeltaTime values. */
  inline DeltaTime operator-(const DeltaTime &o) const
  {
    return DeltaTime(m_time - o.m_time);
  }

  /** @brief Subtracts another DeltaTime from this instance. */
  inline DeltaTime operator-=(const DeltaTime &o)
  {
    m_time -= o.m_time;
    return *this;
  }

  /** @brief Less-than comparison. */
  inline bool operator<(const DeltaTime &o) const { return m_time < o.m_time; }

  /** @brief Greater-than comparison. */
  inline bool operator>(const DeltaTime &o) const { return m_time > o.m_time; }

  /** @brief Less-than-or-equal comparison. */
  inline bool operator<=(const DeltaTime &o) const
  {
    return m_time <= o.m_time;
  }

  /** @brief Greater-than-or-equal comparison. */
  inline bool operator>=(const DeltaTime &o) const
  {
    return m_time >= o.m_time;
  }

  /** Raw time value in nanoseconds. */
  uint64_t m_time;

  /** @return Nanoseconds in one second. */
  inline static constexpr uint64_t oneSecond() { return 1'000'000'000; }

  /** @return Nanoseconds in one millisecond. */
  inline static constexpr uint64_t oneMiliSecond() { return 1'000'000; }

  /** @return Nanoseconds in one microsecond. */
  inline static constexpr uint64_t oneMicroSecond() { return 1'000; }

private:
  static constexpr double sec_d = 1.0 / 1'000'000'000.0;
  static constexpr double milsec_d = 1.0 / 1'000'000.0;
  static constexpr double microsec_d = 1.0 / 1'000.0;

  static constexpr float sec_f = 1.f / 1'000'000'000.f;
  static constexpr float milsec_f = 1.f / 1'000'000.f;
  static constexpr float microsec_f = 1.f / 1'000.f;
};

/** @brief Subtracts a DeltaTime from a raw nanosecond value. */
inline DeltaTime operator-(uint64_t lhs, const DeltaTime &rhs)
{
  return DeltaTime(lhs - rhs.m_time);
}

/** @brief Adds a DeltaTime to a raw nanosecond value. */
inline DeltaTime operator+(uint64_t lhs, const DeltaTime &rhs)
{
  return DeltaTime(lhs + rhs.m_time);
}

/** @brief Scales a DeltaTime by a scalar multiplier. */
inline DeltaTime operator*(const DeltaTime &dt, double scalar)
{
  return DeltaTime(static_cast<uint64_t>((double)dt.m_time * scalar));
}

/** @brief Scales a DeltaTime by a scalar multiplier. */
inline DeltaTime operator*(double scalar, const DeltaTime &dt)
{
  return DeltaTime(static_cast<uint64_t>((double)dt.m_time * scalar));
}

/** @brief Divides a DeltaTime by a scalar value. */
inline DeltaTime operator/(const DeltaTime &dt, double scalar)
{
  return DeltaTime(static_cast<uint64_t>((double)dt.m_time / scalar));
}

} // namespace pain
