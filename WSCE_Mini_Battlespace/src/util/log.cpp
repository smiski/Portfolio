#include "wsce/util/log.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace wsce::log {
static std::mutex g_mu;

static const char* level_str(Level lvl) {
  switch (lvl) {
    case Level::Debug: return "DEBUG";
    case Level::Info:  return "INFO ";
    case Level::Warn:  return "WARN ";
    case Level::Error: return "ERROR";
  }
  return "?????";
}

void write(Level lvl, const std::string& msg) {
  std::lock_guard<std::mutex> lk(g_mu);

  using clock = std::chrono::system_clock;
  const auto t = clock::to_time_t(clock::now());
  std::tm tm{};
#if defined(WSCE_PLATFORM_WINDOWS)
  localtime_s(&tm, &t);
#else
  localtime_r(&t, &tm);
#endif

  std::cerr << std::put_time(&tm, "%H:%M:%S")
            << " [" << level_str(lvl) << "] "
            << msg << "\n";
}
} // namespace wsce::log