/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// LogWrapper.h
#pragma once
#include "pch.h"

#include "Core.h"
#include <spdlog/logger.h>

namespace pain
{

/**
 * @namespace logWrapper
 * @brief Logging subsystem wrapper built on top of spdlog.
 *
 * Provides access to engine, client, and Lua loggers, as well as
 * initialization utilities. Intended to centralize logging configuration
 * and usage across the engine.
 */
namespace logWrapper
{
/**
 * @brief Retrieves the core engine logger.
 *
 * Used for internal engine logging (rendering, systems, assets, etc).
 *
 * @return Reference to the shared pointer holding the core logger.
 */
std::shared_ptr<spdlog::logger> &GetCoreLogger();

/**
 * @brief Retrieves the Lua scripting logger.
 *
 * Intended for logs coming from Lua bindings and scripts.
 *
 * @return Reference to the shared pointer holding the Lua logger.
 */
std::shared_ptr<spdlog::logger> &GetLuaLogger();

/**
 * @brief Retrieves the client / application logger.
 *
 * Intended for user-side or game-side logging.
 *
 * @return Reference to the shared pointer holding the client logger.
 */
std::shared_ptr<spdlog::logger> &GetClientLogger();

/**
 * @brief Initializes all logging backends and registers loggers.
 *
 * Must be called once at startup before using any logging macros.
 * Sets formatting, log levels, and creates the core, client, and Lua loggers.
 */
void InitLogger();
} // namespace logWrapper

} // namespace pain

/** Tell points from vec4 */
#define TP_VEC4(vec) (vec).x, (vec).y, (vec).z, (vec).w
/** Tell points from vec3 */
#define TP_VEC3(vec) (vec).x, (vec).y, (vec).z
/** Tell points from vec2 */
#define TP_VEC2(vec) (vec).x, (vec).y

/** Core trace log macro */
#define PLOG_T(...) ::pain::logWrapper::GetCoreLogger()->trace(__VA_ARGS__)
/** Core info log macro */
#define PLOG_I(...) ::pain::logWrapper::GetCoreLogger()->info(__VA_ARGS__)
/** Core warning log macro */
#define PLOG_W(...) ::pain::logWrapper::GetCoreLogger()->warn(__VA_ARGS__)
/** Core error log macro */
#define PLOG_E(...) ::pain::logWrapper::GetCoreLogger()->error(__VA_ARGS__)
/** Core fatal / critical log macro */
#define PLOG_F(...) ::pain::logWrapper::GetCoreLogger()->critical(__VA_ARGS__)

// clang-format off
/** Lua trace log macro */
#define LUA_LOG_T(...) ::pain::logWrapper::GetLuaLogger()->trace(__VA_ARGS__)
/** Lua info log macro */
#define LUA_LOG_I(...) ::pain::logWrapper::GetLuaLogger()->info(__VA_ARGS__)
/** Lua warning log macro */
#define LUA_LOG_W(...) ::pain::logWrapper::GetLuaLogger()->warn(__VA_ARGS__)
/** Lua error log macro */
#define LUA_LOG_E(...) ::pain::logWrapper::GetLuaLogger()->error(__VA_ARGS__)
/** Lua fatal / critical log macro */
#define LUA_LOG_F(...) ::pain::logWrapper::GetLuaLogger()->critical(__VA_ARGS__)
