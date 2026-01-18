/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "CoreFiles/ThreadPool.h"

ThreadPool::ThreadPool(size_t threadCount)
{
  if (threadCount == 0)
    threadCount = 1;

  // Leave room for the main thread
  if (threadCount > 1)
    threadCount -= 1;

  for (size_t i = 0; i < threadCount; ++i) {
    m_workers.emplace_back(&ThreadPool::workerLoop, this);
  }
}

ThreadPool::~ThreadPool()
{
  {
    std::lock_guard lock(m_mutex);
    m_stopping = true;
  }

  m_cv.notify_all();

  for (auto &t : m_workers) {
    if (t.joinable())
      t.join();
  }
}

void ThreadPool::enqueue(Job job)
{
  {
    std::lock_guard lock(m_mutex);
    m_jobs.push(std::move(job));
  }

  m_cv.notify_one();
}

void ThreadPool::wait()
{
  std::unique_lock lock(m_mutex);
  m_doneCv.wait(lock,
                [this] { return m_jobs.empty() && m_activeJobs.load() == 0; });
}

void ThreadPool::workerLoop()
{
  for (;;) {
    Job job;

    {
      std::unique_lock lock(m_mutex);

      m_cv.wait(lock, [this] { return m_stopping || !m_jobs.empty(); });

      if (m_stopping && m_jobs.empty())
        return;

      job = std::move(m_jobs.front());
      m_jobs.pop();
      ++m_activeJobs;
    }

    // Execute outside lock
    job();

    {
      std::lock_guard lock(m_mutex);
      --m_activeJobs;
    }

    m_doneCv.notify_all();
  }
}
