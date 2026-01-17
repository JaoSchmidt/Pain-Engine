#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * @class ThreadPool
 * @brief Simple fixed-size thread pool for executing background jobs.
 *
 * ThreadPool manages a set of worker threads that continuously pull jobs
 * from a queue and execute them asynchronously. The pool supports job
 * submission and synchronization until all queued and active jobs complete.
 */

// TODO: This was a simple class written by chatgpt, in the future, if needed,
// consider using a library, like EnkiTS

class ThreadPool
{
public:
  /** @brief Job function type executed by the thread pool. */
  using Job = std::function<void()>;

  /**
   * @brief Constructs a thread pool with the specified number of worker
   * threads.
   *
   * If the requested thread count is zero, a minimum of one worker thread
   * is created. One hardware thread is typically reserved for the main thread.
   *
   * @param threadCount Number of worker threads to create.
   */
  explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());

  /**
   * @brief Stops all worker threads and waits for them to terminate.
   *
   * Pending jobs are not executed once destruction begins.
   */
  ~ThreadPool();

  /**
   * @brief Enqueues a job for asynchronous execution.
   *
   * The job will be picked up by the next available worker thread.
   *
   * @param job Callable task to execute.
   */
  void enqueue(Job job);

  /** @brief Blocks until all queued and active jobs are finished. */
  void wait();

private:
  /**
   * @brief Main execution loop for worker threads.
   *
   * Each worker waits for jobs to become available and executes them until
   * the pool is stopped.
   */
  void workerLoop();

private:
  /** Worker threads owned by the pool. */
  std::vector<std::thread> m_workers;
  /** Queue of pending jobs. */
  std::queue<Job> m_jobs;
  /** Mutex protecting the job queue and synchronization state. */
  std::mutex m_mutex;
  /** Condition variable for waking workers when jobs are available. */
  std::condition_variable m_cv;
  /** Condition variable used to signal completion of all jobs. */
  std::condition_variable m_doneCv;
  /** Indicates that the pool is shutting down. */
  std::atomic<bool> m_stopping{false};
  /** Number of jobs currently being executed. */
  std::atomic<size_t> m_activeJobs{0};
};
