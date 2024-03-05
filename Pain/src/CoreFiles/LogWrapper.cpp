#include "CoreFiles/Core.h"
#include <spdlog/spdlog.h>

#include "LogWrapper.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace pain {

std::shared_ptr<spdlog::logger> LogWrapper::s_CoreLogger;
std::shared_ptr<spdlog::logger> LogWrapper::s_ClientLogger;

void LogWrapper::Init() {
  spdlog::set_pattern("%^[%T] %n: %v%$");
  s_CoreLogger = spdlog::stdout_color_mt("PAIN");
  s_CoreLogger->set_level(spdlog::level::trace);

  s_ClientLogger = spdlog::stdout_color_mt("APP");
  s_ClientLogger->set_level(spdlog::level::trace);
}

} // namespace pain
