#pragma once

#define UNUSED(x) (void)(x);
#if defined __linux__
#define PLATFORM_IS_LINUX
#define EXPORT __attribute__((__visibility__("default")))
#elif defined _WIN64
#define PLATFORM_IS_WINDOWS
#define EXPORT __declspec(dllexport)
#define UNUSED(x) (void)(x);
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

#if defined(__GNUC__) || defined(__clang__)
#define PAIN_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define PAIN_FUNC_SIG __FUNCSIG__
#else
#define PAIN_FUNC_SIG __FUNCTION__
#endif
