#pragma once
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace wsce::core {

// A simple thread pool used to parallelize entity updates per tick.
// This reflects typical M&S scaling concerns (more entities => more work).
class ThreadPool {
public:
  explicit ThreadPool(std::size_t threads);
  ~ThreadPool();

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  void enqueue(std::function<void()> job);
  void wait_idle();

private:
  void worker_loop();

  std::mutex m_;
  std::condition_variable cv_;
  std::condition_variable idle_;
  bool stop_{false};

  std::queue<std::function<void()>> q_;
  std::vector<std::thread> workers_;

  std::size_t active_{0};
};

} // namespace wsce::core