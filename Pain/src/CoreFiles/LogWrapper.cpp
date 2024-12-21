#include <spdlog/spdlog.h>

#include "CoreFiles/LogWrapper.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace pain
{

namespace
{
std::shared_ptr<spdlog::logger> s_CoreLogger;
std::shared_ptr<spdlog::logger> s_ClientLogger;
} // namespace

namespace logWrapper
{
// using shared for now
void Init()
{
  spdlog::set_pattern("%^[%T] %n: %v%$");
  s_CoreLogger = spdlog::stdout_color_mt("PAIN");
  s_CoreLogger->set_level(spdlog::level::trace);

  s_ClientLogger = spdlog::stdout_color_mt("APP");
  s_ClientLogger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }
std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }

} // namespace logWrapper

} // namespace pain
