#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// This was a simple class written by chatgpt, in the future, if needed,
// consider using a library, like EnkiTS

class ThreadPool
{
public:
  using Job = std::function<void()>;

  explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
  ~ThreadPool();

  void enqueue(Job job);
  void wait();

private:
  void workerLoop();

private:
  std::vector<std::thread> m_workers;
  std::queue<Job> m_jobs;

  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::condition_variable m_doneCv;

  std::atomic<bool> m_stopping{false};
  std::atomic<size_t> m_activeJobs{0};
};
