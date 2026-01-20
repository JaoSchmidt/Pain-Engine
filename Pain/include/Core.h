/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once
/** Marks a variable or expression as intentionally unused to silence warnings.
 */
#define UNUSED(x) (void)(x);

/** Defined when compiling for Linux platforms. */
#if defined __linux__
#define PLATFORM_IS_LINUX
/** Export attribute for shared library symbols on Linux. */
#define EXPORT __attribute__((__visibility__("default")))
#elif defined _WIN64
/** Defined when compiling for Windows platforms. */
#define PLATFORM_IS_WINDOWS
/** Export attribute for shared library symbols on Windows. */
#define EXPORT __declspec(dllexport)
#else
#error "Only Windows, Linux or MacOS support Pain"
#endif

#ifndef NDEBUG
/**
 * Runtime assertion macro enabled in debug builds.
 *
 * Logs a formatted error message and triggers an assertion failure when the
 * condition evaluates to false.
 *
 * @param x Condition to evaluate.
 * @param s Format string.
 * @param ... Optional format arguments.
 */
#define P_ASSERT(x, s, ...)                                                    \
  {                                                                            \
    if (!(x)) {                                                                \
      PLOG_E("Assertion Failed: " s __VA_OPT__(, ) __VA_ARGS__);               \
      assert(x);                                                               \
    }                                                                          \
  }
/**
 * Warning-only assertion macro enabled in debug builds.
 *
 * Logs a formatted warning message when the condition evaluates to false but
 * does not terminate execution.
 *
 * @param x Condition to evaluate.
 * @param s Format string.
 * @param ... Optional format arguments.
 */
#define P_ASSERT_W(x, s, ...)                                                  \
  {                                                                            \
    if (!(x)) {                                                                \
      PLOG_W("Assertion Failed: " s __VA_OPT__(, ) __VA_ARGS__);               \
    }                                                                          \
  }
#else
/** Disabled assertion macro in release builds. */
#define P_ASSERT(x, ...)
/** Disabled warning assertion macro in release builds. */
#define P_ASSERT_W(x, ...)
#endif

#define NONCOPYABLE(c)                                                         \
  c(const c &o) = delete;                                                      \
  c &operator=(const c &o) = delete;

#define COPYABLE(c)                                                            \
  c(const c &o) = default;                                                     \
  c &operator=(const c &o) = default;

#define MOVABLE(m)                                                             \
  m(m &&o) = default;                                                          \
  m &operator=(m &&o) = default;

#define NONMOVABLE(m)                                                          \
  m(m &&o) = delete;                                                           \
  m &operator=(m &&o) = delete;

/** Expands to a compiler-specific string representing the current function
 * signature. */
#if defined(__GNUC__) || defined(__clang__)
#define PAIN_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define PAIN_FUNC_SIG __FUNCSIG__
#else
#define PAIN_FUNC_SIG __FUNCTION__
#endif
