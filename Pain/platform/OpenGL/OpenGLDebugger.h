/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// OpenGLDebugger.h
namespace pain
{
#include "Core.h"
#include "glad/gl.h"

namespace Debug
{
void glErrorHandler(unsigned int source, unsigned int type, unsigned int id,
                    unsigned int severity, int lenght, const char *message,
                    const void *userParam);
bool glCheckError_(const char *file, int line, const char *func);

#define P_OPENGL_CHECK(...)                                                    \
  if (Debug::glCheckError_(__FILE__, __LINE__, PAIN_FUNC_SIG))                 \
    PLOG_E(__VA_ARGS__);

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define DEBUG_BREAK() __builtin_trap()
#else
#include <csignal>
#define DEBUG_BREAK() raise(SIGTRAP)
#endif

#ifndef NDEBUG
#define GL_CALL(expr)                                                          \
  do {                                                                         \
    expr;                                                                      \
    if (Debug::glCheckError_(__FILE__, __LINE__, PAIN_FUNC_SIG)) {             \
      DEBUG_BREAK();                                                           \
    }                                                                          \
  } while (0)
#else
#define GL_CALL(expr) expr
#endif

} // namespace Debug

} // namespace pain
