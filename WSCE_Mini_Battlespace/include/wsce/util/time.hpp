#pragma once
#include <chrono>

namespace wsce::time {
  using Clock = std::chrono::steady_clock;

  inline double seconds_since(Clock::time_point start) {
    using namespace std::chrono;
    return duration_cast<duration<double>>(Clock::now() - start).count();
  }
}