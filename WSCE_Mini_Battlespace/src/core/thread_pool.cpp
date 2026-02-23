#include "wsce/core/thread_pool.hpp"
#include "wsce/util/log.hpp"

namespace wsce::core {

ThreadPool::ThreadPool(std::size_t threads) {
  if (threads == 0) threads = 4;
  workers_.reserve(threads);
  for (std::size_t i = 0; i < threads; ++i) {
    workers_.emplace_back([this]{ worker_loop(); });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> lk(m_);
    stop_ = true;
  }
  cv_.notify_all();
  for (auto& t : workers_) t.join();
}

void ThreadPool::enqueue(std::function<void()> job) {
  {
    std::lock_guard<std::mutex> lk(m_);
    q_.push(std::move(job));
  }
  cv_.notify_one();
}

void ThreadPool::wait_idle() {
  std::unique_lock<std::mutex> lk(m_);
  idle_.wait(lk, [&]{
    return q_.empty() && active_ == 0;
  });
}

void ThreadPool::worker_loop() {
  for (;;) {
    std::function<void()> job;
    {
      std::unique_lock<std::mutex> lk(m_);
      cv_.wait(lk, [&]{ return stop_ || !q_.empty(); });
      if (stop_ && q_.empty()) return;

      job = std::move(q_.front());
      q_.pop();
      ++active_;
    }

    job();

    {
      std::lock_guard<std::mutex> lk(m_);
      --active_;
      if (q_.empty() && active_ == 0) idle_.notify_all();
    }
  }
}

} // namespace wsce::core