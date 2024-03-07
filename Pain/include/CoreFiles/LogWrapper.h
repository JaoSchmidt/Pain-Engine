#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include <memory>

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

// // Core log macros
#define PLOG_T(...) ::pain::LogWrapper::GetCoreLogger()->trace(__VA_ARGS__)
#define PLOG_I(...) ::pain::LogWrapper::GetCoreLogger()->info(__VA_ARGS__)
#define PLOG_W(...) ::pain::LogWrapper::GetCoreLogger()->warn(__VA_ARGS__)
#define PLOG_E(...) ::pain::LogWrapper::GetCoreLogger()->error(__VA_ARGS__)
#define PLOG_F(...) ::pain::LogWrapper::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define LOG_T(...) ::pain::LogWrapper::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_I(...) ::pain::LogWrapper::GetClientLogger()->info(__VA_ARGS__)
#define LOG_W(...) ::pain::LogWrapper::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_E(...) ::pain::LogWrapper::GetClientLogger()->error(__VA_ARGS__)
#define LOG_F(...) ::pain::LogWrapper::GetClientLogger()->fatal(__VA_ARGS__)
