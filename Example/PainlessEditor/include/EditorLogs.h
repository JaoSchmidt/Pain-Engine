/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// LogWrapper.h
#pragma once

#include <spdlog/logger.h>

namespace painless
{

namespace logWrapper
{
/**
 * @brief Retrieves the editor logger.
 *
 * Compared to internal core logging (rendering, systems, assets, etc),
 * the editor log is only about the editor.
 */
std::shared_ptr<spdlog::logger> &getEditorLogger();

/**
 * @brief Initializes all logging backends and registers loggers.
 *
 * Must be called once at startup before using any logging macros.
 * Sets formatting, log levels, and creates the core, client, and Lua loggers.
 */
void s_initEditorLogger();
} // namespace logWrapper

} // namespace painless

/** Core trace log macro */
#define ELOG_T(...)                                                            \
  ::painless::logWrapper::getEditorLogger()->trace(__VA_ARGS__)
/** Core info log macro */
#define ELOG_I(...) ::painless::logWrapper::getEditorLogger()->info(__VA_ARGS__)
/** Core warning log macro */
#define ELOG_W(...) ::painless::logWrapper::getEditorLogger()->warn(__VA_ARGS__)
/** Core error log macro */
#define ELOG_E(...)                                                            \
  ::painless::logWrapper::getEditorLogger()->error(__VA_ARGS__)
/** Core fatal / critical log macro */
#define ELOG_F(...)                                                            \
  ::painless::logWrapper::getEditorLogger()->critical(__VA_ARGS__)
