#pragma once

#include "pch.h"

namespace pain
{
class DeltaTime
{
public:
  DeltaTime(uint64_t nanoSeconds = 0) : m_time(nanoSeconds) {}
  // clang-format off
  inline double getSeconds() const { return static_cast<double>(m_time) * sec_d; };
  inline double getMiliSeconds() const { return static_cast<double>(m_time) * milsec_d; }
  inline double getMicroSeconds() const { return static_cast<double>(m_time) * microsec_d; }
  inline double getNanoSeconds() const { return static_cast<double>(m_time); }

  inline float getSecondsf() const { return static_cast<float>(m_time) * sec_f; };
  inline float getMiliSecondsf() const { return static_cast<float>(m_time) * milsec_f; }
  inline float getMicroSecondsf() const { return static_cast<float>(m_time) * microsec_f; }
  inline float getNanoSecondsf() const { return static_cast<float>(m_time); }

  inline DeltaTime operator+(const DeltaTime& o) const { return DeltaTime(m_time + o.m_time); }
  inline DeltaTime operator+=(const DeltaTime& o) { m_time += o.m_time; return *this; }
  inline DeltaTime operator-(const DeltaTime& o) const { return DeltaTime(m_time - o.m_time); }
  inline DeltaTime operator-=(const DeltaTime& o) { m_time -= o.m_time; return *this; }

  // comparison
  inline bool operator<(const DeltaTime& o)  const { return m_time < o.m_time; }
  inline bool operator>(const DeltaTime& o)  const { return m_time > o.m_time; }
  inline bool operator<=(const DeltaTime& o) const { return m_time <= o.m_time; }
  inline bool operator>=(const DeltaTime& o) const { return m_time >= o.m_time; }
  uint64_t m_time;

  inline static constexpr uint64_t oneSecond() { return 1'000'000'000; }
  inline static constexpr uint64_t oneMiliSecond() { return 1'000'000; }
  inline static constexpr uint64_t oneMicroSecond() { return 1'000; }
private:
  constexpr static double sec_d = 1.0/1'000'000'000.0;
  constexpr static double milsec_d = 1.0/1'000'000.0;
  constexpr static double microsec_d = 1.0/1'000.0;

  constexpr static float sec_f = 1.f/1'000'000'000.f;
  constexpr static float milsec_f = 1.f/1'000'000.f;
  constexpr static float microsec_f = 1.f/1'000.f;

};

inline DeltaTime operator-(uint64_t lhs, const DeltaTime& rhs)
{
    return DeltaTime(lhs - rhs.m_time);
}
inline DeltaTime operator+(uint64_t lhs, const DeltaTime& rhs)
{
    return DeltaTime(lhs + rhs.m_time);
}

inline DeltaTime operator*(const DeltaTime& dt, double scalar)
{
    return DeltaTime(static_cast<uint64_t>((double)dt.m_time * scalar));
}

inline DeltaTime operator*(double scalar, const DeltaTime& dt)
{
    return DeltaTime(static_cast<uint64_t>((double)dt.m_time * scalar));
}

inline DeltaTime operator/(const DeltaTime& dt, double scalar)
{
    return DeltaTime(static_cast<uint64_t>((double)dt.m_time / scalar));
}



} // namespace pain
