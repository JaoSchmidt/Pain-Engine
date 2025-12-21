#include <spdlog/spdlog.h>

#include "CoreFiles/LogWrapper.h"

#include "spdlog/common.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace pain
{

namespace
{
std::shared_ptr<spdlog::logger> s_CoreLogger;
std::shared_ptr<spdlog::logger> s_ClientLogger;
std::shared_ptr<spdlog::logger> s_LuaLogger;
} // namespace

namespace logWrapper
{
// using shared for now
void InitLogger()
{
  spdlog::set_pattern("%^[%T] %n: %v%$");
  s_CoreLogger = spdlog::stdout_color_mt("PAIN");
  s_CoreLogger->set_level(spdlog::level::trace);

  s_ClientLogger = spdlog::stdout_color_mt("APP");
  s_ClientLogger->set_level(spdlog::level::trace);

  // ---- Lua logger with custom colors ----
  auto lua_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

// NOTE: this is exactly how is defined inside spdlog
#ifdef _WIN32
  lua_sink->set_color(spdlog::level::info, FOREGROUND_BLUE);
#else
  lua_sink->set_color(spdlog::level::info, lua_sink->blue);
#endif

  s_LuaLogger = std::make_shared<spdlog::logger>("LUA", lua_sink);
  s_LuaLogger->set_level(spdlog::level::trace);
  s_LuaLogger->set_formatter(
      spdlog::details::make_unique<spdlog::pattern_formatter>(
          "%^[%T] %n: %v%$"));

  spdlog::register_logger(s_LuaLogger);
}

std::shared_ptr<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }
std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
std::shared_ptr<spdlog::logger> &GetLuaLogger() { return s_LuaLogger; }

} // namespace logWrapper

} // namespace pain
