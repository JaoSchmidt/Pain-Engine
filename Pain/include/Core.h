#pragma once

#ifdef IS_LIB_STATIC
#define
#elif __linux__
#define PLATFORM_IS_LINUX
#define EXPORT __attribute__((__visibility__("default")))
#define UNUSED __attribute__((unused))
#define UNUSED_PARM(x) (void)(x)
#elif defined _WIN64
#define PLATFORM_IS_WINDOWS
#define EXPORT __declspec(dllexport)
#define UNUSED
#else
#error "Only Windows, Linux or MacOS support Pain"
#endif

#ifndef NDEBUG
#define P_ASSERT(x, s, ...)                                                    \
  {                                                                            \
    if (!(x)) {                                                                \
      PLOG_E("Assertion Failed: " s __VA_OPT__(, ) __VA_ARGS__);               \
      assert(x);                                                               \
    }                                                                          \
  }
#define P_ASSERT_W(x, s, ...)                                                  \
  {                                                                            \
    if (!(x)) {                                                                \
      PLOG_W("Assertion Failed: " s __VA_OPT__(, ) __VA_ARGS__);               \
    }                                                                          \
  }
#else
#define P_ASSERT(x, ...)
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
