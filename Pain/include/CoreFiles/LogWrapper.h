#pragma once
#include "pch.h"

#include "Core.h"
#include <spdlog/logger.h>

namespace pain
{

class EXPORT LogWrapper
{
public:
  inline static std::shared_ptr<spdlog::logger> &GetClientLogger()
  {
    return s_ClientLogger;
  }

  inline static std::shared_ptr<spdlog::logger> &GetCoreLogger()
  {
    return s_CoreLogger;
  }
  static void Init();

private:
  static std::shared_ptr<spdlog::logger> s_CoreLogger;
  static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

} // namespace pain

// Tell points from vec4
#define TP_VEC4(vec) (vec).x, (vec).y, (vec).z, (vec).w
// Tell points from vec3
#define TP_VEC3(vec) (vec).x, (vec).y, (vec).z
// Tell points from vec2
#define TP_VEC2(vec) (vec).x, (vec).y
// Core log macros
#define PLOG_T(...) ::pain::LogWrapper::GetCoreLogger()->trace(__VA_ARGS__)
#define PLOG_I(...) ::pain::LogWrapper::GetCoreLogger()->info(__VA_ARGS__)
#define PLOG_W(...) ::pain::LogWrapper::GetCoreLogger()->warn(__VA_ARGS__)
#define PLOG_E(...) ::pain::LogWrapper::GetCoreLogger()->error(__VA_ARGS__)
#define PLOG_F(...) ::pain::LogWrapper::GetCoreLogger()->critical(__VA_ARGS__)
