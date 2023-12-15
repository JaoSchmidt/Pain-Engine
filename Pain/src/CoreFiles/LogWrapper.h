#pragma once

#include <memory>

#include "Core.h"
#include "spdlog/spdlog.h"

namespace pain {

class LogWrapper {
public:
  static void Init();

  inline static std::shared_ptr<spdlog::logger> &GetCoreLogger() {
    return s_CoreLogger;
  }
  inline static std::shared_ptr<spdlog::logger> &GetClientLogger() {
    return s_ClientLogger;
  }

private:
  static std::shared_ptr<spdlog::logger> s_CoreLogger;
  static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

} // namespace pain

// Core log macros
#define LOG_TRACE_C(...) ::pain::LogWrapper::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_INFO_C(...) ::pain::LogWrapper::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_WARN_C(...) ::pain::LogWrapper::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_ERROR_C(...) ::pain::LogWrapper::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_FATAL_C(...) ::pain::LogWrapper::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define LOG_TRACE(...) ::pain::LogWrapper::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::pain::LogWrapper::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::pain::LogWrapper::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::pain::LogWrapper::GetClientLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...) ::pain::LogWrapper::GetClientLogger()->fatal(__VA_ARGS__)
