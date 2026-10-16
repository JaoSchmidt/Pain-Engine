/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// Profiling.cpp
#include "Debugging/Profiling.h"
#include "CoreFiles/LogWrapper.h"

#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <atomic>
#include <charconv>
#include <cmath>
#include <fstream>
#include <mutex>

#include <thread>

namespace pain
{

namespace // profiler variables
{
constexpr uint64_t NO_SESSION = 0;

/**
 * Amount of serialized events buffered before they are handed to the file.
 *
 * Batching is what keeps a ProfileTimer destructor cheap: one write() syscall
 * per batch instead of one per event.
 */
constexpr size_t FLUSH_THRESHOLD = 64 * 1024;

ProfileStream *m_currentSession = nullptr;
std::atomic<uint64_t> m_currentSessionID{NO_SESSION};
uint64_t m_nextSessionID = NO_SESSION + 1;
uint64_t m_perfFrequency = 0;
uint64_t m_zeroTime = 0;
std::ofstream m_outputStream;
/**
 * Events serialized but not yet handed to m_outputStream.
 *
 * Also serves as the reusable scratch space for formatting, so no event needs
 * a fresh heap allocation.
 */
std::string m_pendingBuffer;
// recursive, because open/closeStream hold it while writing header/footer
std::recursive_mutex m_outputMutex;
uint64_t m_profileCount = 0;
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
 * @return Duration in microseconds, 0 if the frequency is unknown.
 */
double ticksToMicroSeconds(uint64_t ticks)
{
  if (m_perfFrequency == 0)
    return 0.0;
  return (static_cast<double>(ticks) / static_cast<double>(m_perfFrequency)) *
         1'000'000.0;
}

/**
 * @brief Hands every buffered event to the output stream in a single write.
 *
 * The caller must hold m_outputMutex.
 */
void flushPendingBuffer()
{
  const size_t size = m_pendingBuffer.size();
  if (size == 0)
    return;

  m_outputStream.write(m_pendingBuffer.data(),
                       static_cast<std::streamsize>(size));
  if (!m_outputStream)
    PLOG_E("Profiling stream write failed, discarded {} buffered bytes", size);
  m_pendingBuffer.clear();
}

/**
 * @brief Appends an unsigned integer without allocating.
 */
template <typename T> void appendUnsigned(std::string &out, T value)
{
  char buffer[24];
  const auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);
  out.append(buffer, result.ptr);
}

/**
 * @brief Appends a timestamp or duration in microseconds.
 *
 * The trace format wants microseconds, and 6 decimals give picosecond
 * resolution, far below the millisecond that has to stay precise.
 * std::chars_format::fixed keeps the output in plain decimal notation: it never
 * switches to scientific notation. Non-finite input is written as 0 so the
 * result stays valid JSON instead of turning into inf or nan.
 */
void appendMicroSeconds(std::string &out, double microSeconds)
{
  if (!std::isfinite(microSeconds))
    microSeconds = 0.0;

  // Room for the largest finite double (309 integer digits) in fixed notation
  // with 6 decimals, plus a sign.
  char buffer[320];
  const auto result = std::to_chars(buffer, buffer + sizeof(buffer),
                                    microSeconds, std::chars_format::fixed, 6);
  if (result.ec == std::errc{})
    out.append(buffer, result.ptr);
  else
    out += '0';
}

/**
 * @brief Reports whether a character cannot go into a JSON string verbatim.
 */
bool needsJsonEscape(char c)
{
  return c == '"' || c == '\\' || static_cast<unsigned char>(c) < 0x20;
}

/**
 * @brief Appends a scope name as the body of a JSON string, without quotes.
 *
 * Names nearly never contain a character that needs escaping, so the common
 * case copies the name in one go instead of one character at a time.
 */
void appendJsonName(std::string &out, const char *name)
{
  constexpr char hexDigits[] = "0123456789abcdef";

  const size_t length = std::char_traits<char>::length(name);
  const char *const end = name + length;

  const char *cursor = std::find_if(name, end, needsJsonEscape);
  if (cursor == end) { // nothing to escape, the usual case
    out.append(name, length);
    return;
  }

  out.append(name, static_cast<size_t>(cursor - name));
  for (; cursor != end; ++cursor) {
    switch (*cursor) {
    case '"':
      out += "\\\"";
      break;
    case '\\':
      out += "\\\\";
      break;
    default: {
      const auto value = static_cast<unsigned char>(*cursor);
      if (value < 0x20) { // remaining control characters need a \u escape
        out += "\\u00";
        out += hexDigits[value >> 4];
        out += hexDigits[value & 0x0F];
      } else {
        out += *cursor;
      }
      break;
    }
    }
  }
}
} // namespace

namespace Profiler
{

void writeHeader()
{
  const std::lock_guard lock(m_outputMutex);
  m_pendingBuffer.clear();
  m_profileCount = 0;
  m_outputStream << "{\"otherData\": {},\"traceEvents\":[";
}

void writeFooter()
{
  const std::lock_guard lock(m_outputMutex);
  // drain first, the closing bracket has to come after the last event
  flushPendingBuffer();
  m_outputStream << "]}";
  m_outputStream.flush();
}

void writeProfile(const ProfileResult &result)
{
  const std::lock_guard lock(m_outputMutex);

  // read under the lock, so a concurrently opened session cannot mix its zero
  // time into this event
  const double durationMicro =
      ticksToMicroSeconds(ticksSince(result.m_end, result.m_start));
  const double startMicro =
      ticksToMicroSeconds(ticksSince(result.m_start, m_zeroTime));

  // Serialize into the batch buffer. Deliberately no stream flush here: this
  // runs on the ProfileTimer destructor path, and a flush would put a write()
  // syscall into every enclosing scope's measurement.
  std::string &out = m_pendingBuffer;

  if (m_profileCount++ > 0) {
    out += ',';
  }

  out += "{\"cat\":\"function\",\"dur\":";
  appendMicroSeconds(out, durationMicro);
  out += ",\"name\":\"";
  appendJsonName(out, result.m_name);
  out += "\",\"ph\":\"X\",\"pid\":0,\"tid\":";
  appendUnsigned(out, result.m_threadID);
  out += ",\"ts\":";
  appendMicroSeconds(out, startMicro);
  out += '}';

  if (out.size() >= FLUSH_THRESHOLD)
    flushPendingBuffer();
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

  // Numbers go through std::to_chars instead of stream manipulators, see
  // appendMicroSeconds for why plain decimal notation matters here.
  m_pendingBuffer.reserve(FLUSH_THRESHOLD + 4096);

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
  m_pendingBuffer.clear();
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
