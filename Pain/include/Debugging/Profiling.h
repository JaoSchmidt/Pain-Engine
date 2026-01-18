/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// based on cherno's profiler
// https://gist.github.com/TheCherno/31f135eea6ee729ab5f26a6908eb3a5e

// Profiling.h
#pragma once

#include "Assets/DeltaTime.h"
#include "Core.h"

namespace pain
{
/**
 * @brief Represents a single profiling measurement result.
 *
 * A ProfileResult contains the timing information for one profiled scope,
 * including the start and end timestamps and the originating thread.
 *
 * This structure is typically produced internally by ProfileTimer and
 * forwarded to the profiler backend for serialization.
 */
struct ProfileResult {
  /// Name of the profiled scope or function.
  const char *m_name;
  /// Timestamp at which the scope started.
  DeltaTime m_start;
  /// Timestamp at which the scope ended.
  DeltaTime m_end;
  /// Hash of the thread ID that produced this profile event.
  size_t m_threadID;
};

/**
 * @brief Represents an active profiling session.
 *
 * A ProfileStream identifies a profiling capture session and is associated
 * with a single output file. Only one profiling stream may be active at a time.
 *
 * This type is primarily used internally by the profiler implementation.
 */
struct ProfileStream {
  /// Name of the active profiling session.
  const char *m_name;
};

namespace Profiler
{
/**
 * @brief Writes a profiling result to the active profiling stream.
 *
 * This function serializes the provided ProfileResult and appends it to the
 * currently open profiling output file. If no stream is active, the call has
 * no effect.
 *
 * @param result Profiling data to record.
 */
void writeProfile(const ProfileResult &result);
/**
 * @brief Opens a new profiling stream and output file.
 *
 * Creates or overwrites the specified output file and prepares it to receive
 * profiling events. Only one stream can be active at a time; opening a new
 * stream while another is active will overwrite the previous state.
 *
 * @param name     Human-readable name of the profiling session.
 * @param filepath Path to the output file. Defaults to "results.json".
 */
void openStream(const char *name, const char *filepath = "results.json");

/**
 * @brief Closes the currently active profiling stream.
 *
 * Finalizes the output file and releases any internal resources associated
 * with the active profiling session. If no stream is active, this function
 * has no effect.
 */
void closeStream();

}; // namespace Profiler

/**
 * @brief RAII timer used to measure the execution time of a scope.
 *
 * When constructed, the timer records the current timestamp. When the timer
 * is destroyed or Stop() is called explicitly, the elapsed time is calculated
 * and written to the active profiling stream.
 *
 * Typical usage is through the PROFILE_SCOPE or PROFILE_FUNCTION macros,
 * which automatically manage the timer lifetime.
 *
 * If no profiling stream is currently active, the timer performs no work.
 */
class ProfileTimer
{
public:
  /**
   * @brief Constructs a new profiling timer for a named scope.
   *
   * @param name Name of the scope being profiled.
   */
  ProfileTimer(const char *name);

  /**
   * @brief Destroys the timer and records the result if not already stopped.
   */
  ~ProfileTimer();

  /**
   * @brief Stops the timer and records the profiling result.
   *
   * Calling Stop() multiple times is safe; only the first call records a
   * result.
   */
  void Stop();

private:
  /// Name of the profiled scope.
  const char *m_name = "undefined";

  /// Timestamp captured at construction time.
  DeltaTime m_start = 0;

  /// Indicates whether the timer has already been stopped.
  bool m_stopped = true;
};

} // namespace pain

#ifdef PROFILING
/**
 * @brief Opens a profiling session.
 *
 * Convenience macro that forwards to pain::Profiler::openStream().
 */
#define PROFILE_OPEN(name, filepath)                                           \
  ::pain::Profiler::openStream(name, filepath)

/**
 * @brief Closes the active profiling session.
 *
 * Convenience macro that forwards to pain::Profiler::closeStream().
 */
#define PROFILE_CLOSE() ::pain::Profiler::closeStream()

/**
 * @brief Profiles the lifetime of the current scope.
 *
 * Creates a scoped ProfileTimer that measures the duration of the enclosing
 * scope.
 *
 * @param name Name to associate with the profiled scope.
 */
#define PROFILE_SCOPE(name) ::pain::ProfileTimer timer##__LINE__(name);

/**
 * @brief Profiles the current function.
 *
 * Expands to PROFILE_SCOPE using the compiler-provided function signature.
 */
#define PROFILE_FUNCTION() PROFILE_SCOPE(PAIN_FUNC_SIG)

#else
#define PROFILE_OPEN(name, filepath)
#define PROFILE_CLOSE()
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif
