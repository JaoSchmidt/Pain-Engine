// based on cherno's profiler
// https://gist.github.com/TheCherno/31f135eea6ee729ab5f26a6908eb3a5e
#pragma once

#include "Core.h"
#include "CoreFiles/DeltaTime.h"

namespace pain
{

struct ProfileResult {
  const char *m_name;
  DeltaTime m_start, m_end;
  size_t m_threadID;
};

struct ProfileStream {
  const char *m_name;
};

namespace Profiler
{

void writeHeader();
void writeFooter();
void writeProfile(const ProfileResult &result);
void openStream(const char *name, const char *filepath = "results.json");
void closeStream();

}; // namespace Profiler
class ProfileTimer
{
public:
  ProfileTimer(const char *name);
  ~ProfileTimer();
  void Stop();

private:
  const char *m_name = "undefined";
  DeltaTime m_start = 0;
  bool m_stopped = true;
};
} // namespace pain

#ifndef NDEBUG
#define PROFILE_OPEN(name, filepath)                                           \
  ::pain::Profiler::openStream(name, filepath)
#define PROFILE_CLOSE() ::pain::Profiler::closeStream()
#define PROFILE_SCOPE(name) ::pain::ProfileTimer timer##__LINE__(name);
#define PROFILE_FUNCTION() PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
#define PROFILE_OPEN(name, filepath)
#define PROFILE_CLOSE()
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif
