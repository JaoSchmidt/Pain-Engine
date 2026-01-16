/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#pragma once
#include "pch.h"

#include "Core.h"
#include <spdlog/logger.h>

namespace pain
{

namespace logWrapper
{
std::shared_ptr<spdlog::logger> &GetCoreLogger();
std::shared_ptr<spdlog::logger> &GetLuaLogger();
std::shared_ptr<spdlog::logger> &GetClientLogger();
void InitLogger();
} // namespace logWrapper

} // namespace pain

// Tell points from vec4
#define TP_VEC4(vec) (vec).x, (vec).y, (vec).z, (vec).w
// Tell points from vec3
#define TP_VEC3(vec) (vec).x, (vec).y, (vec).z
// Tell points from vec2
#define TP_VEC2(vec) (vec).x, (vec).y
// Core log macros
#define PLOG_T(...) ::pain::logWrapper::GetCoreLogger()->trace(__VA_ARGS__)
#define PLOG_I(...) ::pain::logWrapper::GetCoreLogger()->info(__VA_ARGS__)
#define PLOG_W(...) ::pain::logWrapper::GetCoreLogger()->warn(__VA_ARGS__)
#define PLOG_E(...) ::pain::logWrapper::GetCoreLogger()->error(__VA_ARGS__)
#define PLOG_F(...) ::pain::logWrapper::GetCoreLogger()->critical(__VA_ARGS__)

// clang-format off
#define LUA_LOG_T(...) ::pain::logWrapper::GetLuaLogger()->trace(__VA_ARGS__)
#define LUA_LOG_I(...) ::pain::logWrapper::GetLuaLogger()->info(__VA_ARGS__)
#define LUA_LOG_W(...) ::pain::logWrapper::GetLuaLogger()->warn(__VA_ARGS__)
#define LUA_LOG_E(...) ::pain::logWrapper::GetLuaLogger()->error(__VA_ARGS__)
#define LUA_LOG_F(...) ::pain::logWrapper::GetLuaLogger()->critical(__VA_ARGS__)
