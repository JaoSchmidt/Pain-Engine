/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Profiling.cpp
#if 0
#include "Debugging/ProfilingMutex.h"
#include "CoreFiles/LogWrapper.h"

#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <atomic>
#include <fstream>
#include <iomanip>
#include <mutex>

#include <thread>

namespace pain
{

namespace // profiler variables
{
constexpr uint64_t NO_SESSION = 0;

ProfileStream *m_currentSession = nullptr;
std::atomic<uint64_t> m_currentSessionID{NO_SESSION};
uint64_t m_nextSessionID = NO_SESSION + 1;
uint64_t m_perfFrequency = 0;
uint64_t m_zeroTime = 0;
std::ofstream m_outputStream;
// recursive, because open/closeStream hold it while writing header/footer
std::recursive_mutex m_outputMutex;
int m_profileCount = 0;
} // namespace

namespace // profiler helpers
{
/**
 * @return Amount of ticks elapsed between base and ticks.
 *
 * Saturates at 0 instead of wrapping around, so an event can never end up
 * with a nonsense (~1.8e19) duration or timestamp.
 */
uint64_t ticksSince(uint64_t ticks, uint64_t base)
{
  return ticks > base ? ticks - base : 0;
}

/**
 * @brief Converts raw performance counter ticks into microseconds.
 *
 * The Chrome Trace Event format defines "ts" and "dur" in microseconds, so
 * the values must be emitted in that unit - a viewer reads them as
 * microseconds and quantizes them accordingly.
 *
 * The division is done before the multiplication to keep the intermediate
 * value small, which keeps the result precise (down to the last tick, and
 * far below a microsecond) even for very long running sessions. The largest
 * possible result is bounded by 2^64 ticks, so it always stays finite.
 *
 * @return Duration in microseconds, 0 if the frequency is unknown.
 */
double ticksToMicroSeconds(uint64_t ticks)
{
  if (m_perfFrequency == 0)
    return 0.0;
  return (static_cast<double>(ticks) / static_cast<double>(m_perfFrequency)) *
         1'000'000.0;
}
} // namespace

namespace Profiler
{

void writeHeader()
{
  const std::lock_guard lock(m_outputMutex);
  m_outputStream << "{\"otherData\": {},\"traceEvents\":[";
  m_outputStream.flush();
  m_profileCount = 0;
}

void writeFooter()
{
  const std::lock_guard lock(m_outputMutex);
  m_outputStream << "]}";
  m_outputStream.flush();
}

void writeProfile(const ProfileResult &result)
{
  std::string name = result.m_name;
  std::replace(name.begin(), name.end(), '"', '\'');

  const std::lock_guard lock(m_outputMutex);

  // read under the lock, so a concurrently opened session cannot mix its zero
  // time into this event
  const double durationMicro =
      ticksToMicroSeconds(ticksSince(result.m_end, result.m_start));
  const double startMicro =
      ticksToMicroSeconds(ticksSince(result.m_start, m_zeroTime));

  if (m_profileCount++ > 0) {
    m_outputStream << ",";
  }

  m_outputStream << "{";
  m_outputStream << "\"cat\":\"function\",";
  m_outputStream << "\"dur\":" << durationMicro << ',';
  m_outputStream << "\"name\":\"" << name << "\",";
  m_outputStream << "\"ph\":\"X\",";
  m_outputStream << "\"pid\":0,";
  m_outputStream << "\"tid\":" << result.m_threadID << ",";
  m_outputStream << "\"ts\":" << startMicro;
  m_outputStream << "}";

  m_outputStream.flush();
}

void openStream(const char *name, const char *filepath)
{
  const std::lock_guard lock(m_outputMutex);

  const auto frequency = static_cast<uint64_t>(SDL_GetPerformanceFrequency());
  if (frequency == 0) {
    PLOG_E("Cannot start profiling session \"{}\": SDL reported a performance "
           "frequency of 0",
           name);
    return;
  }

  closeStream(); // finish a possibly still running session first

  m_outputStream.open(filepath);
  if (!m_outputStream.is_open()) {
    PLOG_E("Cannot start profiling session \"{}\": failed to open \"{}\"", name,
           filepath);
    return;
  }

  // The trace format wants microseconds. 6 decimals give picosecond
  // resolution, far below the 1 millisecond that has to stay precise, and
  // std::fixed keeps the output in plain decimal notation: it never switches to
  // scientific notation and never emits inf, no matter how large the value is.
  m_outputStream << std::fixed << std::setprecision(6);

  m_perfFrequency = frequency;
  // must be set before the session becomes visible, otherwise a timer created
  // in between would compute its timestamp relative to an uninitialized zero
  m_zeroTime = SDL_GetPerformanceCounter();
  m_currentSession = new ProfileStream{name};
  m_currentSessionID.store(m_nextSessionID++, std::memory_order_release);
  writeHeader();
}

void closeStream()
{
  const std::lock_guard lock(m_outputMutex);

  if (m_currentSession == nullptr)
    return;

  if (m_outputStream.is_open()) {
    writeFooter();
    m_outputStream.close();
  }
  PLOG_T("Closing stream \"{}\"", m_currentSession->m_name);
  delete m_currentSession;
  m_currentSession = nullptr;
  m_currentSessionID.store(NO_SESSION, std::memory_order_release);
  m_profileCount = 0;
}

} // namespace Profiler

ProfileTimer::ProfileTimer(const char *name) : m_name(name)
{
  const uint64_t sessionID = m_currentSessionID.load(std::memory_order_acquire);
  if (sessionID == NO_SESSION)
    return; // stay unarmed: no session, so nothing to report

  m_sessionID = sessionID;
  m_start = SDL_GetPerformanceCounter();
  m_stopped = false;
}

ProfileTimer::~ProfileTimer()
{
  if (!m_stopped)
    Stop();
}

void ProfileTimer::Stop()
{
  if (m_stopped)
    return;
  m_stopped = true;

  // Only report into the very session this timer was started in. If it was
  // closed or replaced meanwhile, the timestamps cannot be related to the
  // zero time of the current session.
  if (m_sessionID == NO_SESSION ||
      m_sessionID != m_currentSessionID.load(std::memory_order_acquire))
    return;

  const uint64_t end = SDL_GetPerformanceCounter();
  const size_t threadID =
      std::hash<std::thread::id>{}(std::this_thread::get_id());
  Profiler::writeProfile({m_name, m_start, end, threadID});
}

} // namespace pain
#endif
