#pragma once

#include "pch.h"

namespace pain
{
class DeltaTime
{
private:
  const static int sec_d = 1000000000;
  const static int milsec_d = 1000000;

public:
  DeltaTime(uint64_t time = 0) : m_time(time) {}
  inline double GetSeconds() const { return (double)m_time / sec_d; };
  inline double GetMiliseconds() const { return (double)m_time / milsec_d; }

  inline DeltaTime operator+(const DeltaTime &o) const
  {
    return DeltaTime(m_time + o.m_time);
  }
  inline DeltaTime operator+(const uint64_t &o) const
  {
    return DeltaTime(m_time + o);
  }

  inline DeltaTime operator-(const DeltaTime &o) const
  {
    return DeltaTime(m_time - o.m_time);
  }
  inline DeltaTime operator-(const uint64_t &o) const
  {
    return DeltaTime(m_time - o);
  }
  inline operator uint64_t() const { return m_time; }

private:
  uint64_t m_time;
};
} // namespace pain
