/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Assets/DeltaTime.h"
#include <sol/sol.hpp>
namespace pain
{

void luabinder::bindDeltaTime(sol::state &lua)
{
  lua.new_usertype<DeltaTime>(
      "DeltaTime",
      // constructors
      sol::constructors<DeltaTime(), DeltaTime(uint64_t)>(),

      // static factory
      "create_seconds", &DeltaTime::createSeconds,

      // getters (double)
      "seconds", &DeltaTime::getSeconds, "milliseconds",
      &DeltaTime::getMilliSeconds, "microseconds", &DeltaTime::getMicroSeconds,
      "nanoseconds", &DeltaTime::getNanoSeconds,

      // getters (float)
      "secondsf", &DeltaTime::getSecondsf, "millisecondsf",
      &DeltaTime::getMilliSecondsf, "microsecondsf",
      &DeltaTime::getMicroSecondsf, "nanosecondsf", &DeltaTime::getNanoSecondsf,

      // raw value (optional exposure)
      "raw", &DeltaTime::m_time,

      // operators
      sol::meta_function::addition,
      sol::overload(
          [](const DeltaTime &a, const DeltaTime &b) { return a + b; }),

      sol::meta_function::subtraction,
      sol::overload(
          [](const DeltaTime &a, const DeltaTime &b) { return a - b; }),

      sol::meta_function::multiplication,
      sol::overload([](const DeltaTime &dt, double s) { return dt * s; },
                    [](double s, const DeltaTime &dt) { return s * dt; }),

      sol::meta_function::division,
      [](const DeltaTime &dt, double s) { return dt / s; },

      // comparisons
      sol::meta_function::less_than, &DeltaTime::operator<,
      sol::meta_function::less_than_or_equal_to, &DeltaTime::operator<=,
      sol::meta_function::equal_to,
      [](const DeltaTime &a, const DeltaTime &b) {
        return a.m_time == b.m_time;
      });
}
} // namespace pain
