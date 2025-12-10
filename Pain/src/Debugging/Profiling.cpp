#include "Debugging/Profiling.h"
#include "CoreFiles/LogWrapper.h"

#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <fstream>

#include <thread>

namespace pain
{

namespace // profiler variables
{
ProfileStream *m_currentSession = nullptr;
std::ofstream m_outputStream;
int m_profileCount = 0;
DeltaTime m_zeroTime;
} // namespace

namespace Profiler
{

void writeHeader()
{
  m_outputStream << "{\"otherData\": {},\"traceEvents\":[";
  m_outputStream.flush();
  m_profileCount = 0;
}

void writeFooter()
{
  m_outputStream << "]}";
  m_outputStream.flush();
}

void openStream(const char *name, const char *filepath)
{
  m_outputStream.open(filepath);
  writeHeader();
  m_currentSession = new ProfileStream{name};
  m_zeroTime = SDL_GetPerformanceCounter();
}

void closeStream()
{
  if (m_outputStream.is_open()) {
    writeFooter();
    m_outputStream.close();
    PLOG_T("Closing stream \"{}\"", m_currentSession->m_name);
    delete m_currentSession;
    m_currentSession = nullptr;
    m_profileCount = 0;
  }
}

void writeProfile(const ProfileResult &result)
{
  if (m_profileCount++ > 0) {
    m_outputStream << ",";
  }

  std::string name = result.m_name;
  std::replace(name.begin(), name.end(), '"', '\'');
  double freq = (double)SDL_GetPerformanceFrequency();
  const DeltaTime duration = result.m_end - result.m_start;
  const DeltaTime start = result.m_start - m_zeroTime;

  const double durationMili =
      (double)(duration.getNanoSeconds() * 1'000'000.0 / freq);
  const double startMili =
      (double)(start.getNanoSeconds() * 1'000'000.0 / freq);

  m_outputStream << "{";
  m_outputStream << "\"cat\":\"function\",";
  m_outputStream << "\"dur\":" << durationMili << ',';
  m_outputStream << "\"name\":\"" << name << "\",";
  m_outputStream << "\"ph\":\"X\",";
  m_outputStream << "\"pid\":0,";
  m_outputStream << "\"tid\":" << result.m_threadID << ",";
  m_outputStream << "\"ts\":" << startMili;
  m_outputStream << "}";

  m_outputStream.flush();
}
} // namespace Profiler

ProfileTimer::ProfileTimer(const char *name) : m_name(name), m_stopped(false)
{
  if (m_currentSession == nullptr)
    return;
  m_start = SDL_GetPerformanceCounter();
}

ProfileTimer::~ProfileTimer()
{
  if (!m_stopped)
    Stop();
}

void ProfileTimer::Stop()
{
  if (m_currentSession != nullptr) {
    unsigned long end = SDL_GetPerformanceCounter();
    size_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
    Profiler::writeProfile({m_name, m_start, end, threadID});
  }
  m_stopped = true;
}

} // namespace pain
