#pragma once
#include <cstdint>

namespace wsce::config {
  // Real-time simulation defaults
  constexpr double kFixedDtSec = 0.02;     // 50 Hz
  constexpr double kMaxSimSec  = 60.0;     // demo run time
  constexpr int    kWorkerThreads = 0;     // 0 => auto

  // Networking defaults
  constexpr uint16_t kTelemetryPort = 49000;
  constexpr uint16_t kCommandPort   = 49001;
}