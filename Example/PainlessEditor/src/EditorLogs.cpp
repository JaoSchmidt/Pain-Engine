#include "EditorLogs.h"
#include <spdlog/spdlog.h>

#include "spdlog/common.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace painless
{

namespace
{
std::shared_ptr<spdlog::logger> s_EditorLogger = nullptr;
}
namespace logWrapper
{

void s_initEditorLogger()
{
  if (!s_EditorLogger) {
    auto editorSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

#ifdef _WIN32
    editorSink->set_color(spdlog::level::info, FOREGROUND_MAGENTA);
#else
    editorSink->set_color(spdlog::level::info, editorSink->magenta);
#endif
    s_EditorLogger = std::make_shared<spdlog::logger>("EDITOR", editorSink);
    s_EditorLogger->set_level(spdlog::level::trace);
    s_EditorLogger->set_formatter(
        spdlog::details::make_unique<spdlog::pattern_formatter>(
            "%^[%T] %n: %v%$"));

    spdlog::register_logger(s_EditorLogger);
  }
}

std::shared_ptr<spdlog::logger> &getEditorLogger() { return s_EditorLogger; }
} // namespace logWrapper
} // namespace painless
