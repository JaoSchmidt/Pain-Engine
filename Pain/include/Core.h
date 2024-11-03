#pragma once
#include <string>

#ifdef IS_LIB_STATIC
#define EXPORT
#elif __linux__
#define PLATFORM_IS_LINUX
#define EXPORT __attribute__((__visibility__("default")))
#elif defined _WIN64
#define PLATFORM_IS_WINDOWS
#define EXPORT __declspec(dllexport)
#else
#error "Only Windows or Linux supports Pain"
#endif

#ifndef NDEBUG
#define P_ASSERT(x, s, ...)                                                    \
  {                                                                            \
    if (!(x)) {                                                                \
      PLOG_E("Assertion Failed: " s, ##__VA_ARGS__);                           \
      assert(x);                                                               \
    }                                                                          \
  }
#define P_ASSERT_W(x, s, ...)                                                  \
  {                                                                            \
    if (!(x)) {                                                                \
      PLOG_W("Assertion Failed: " s, ##__VA_ARGS__);                           \
    }                                                                          \
  }
#else
#define P_ASSERT(x, ...)
#endif

#define NONCOPYABLE(c)                                                         \
  c(const c &) = delete;                                                       \
  c &operator=(const c &) = delete;

#define COPIES(c)                                                              \
  c(const c &);                                                                \
  c &operator=(const c &);
