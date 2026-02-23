#include "wsce/net/telemetry.hpp"
#include <sstream>

namespace wsce::net {

std::string encode_snapshot_json(const std::vector<wsce::sim::World::SnapshotItem>& items,
                                 double simTimeSec) {
  std::ostringstream oss;
  oss.precision(3);
  oss << "{"
      << "\"t\":" << simTimeSec << ","
      << "\"entities\":[";
  for (size_t i = 0; i < items.size(); ++i) {
    const auto& it = items[i];
    oss << "{"
        << "\"id\":" << it.id << ","
        << "\"name\":\"" << it.name << "\","
        << "\"pos\":[" << it.state.pos.x << "," << it.state.pos.y << "," << it.state.pos.z << "],"
        << "\"vel\":[" << it.state.vel.x << "," << it.state.vel.y << "," << it.state.vel.z << "]"
        << "}";
    if (i + 1 < items.size()) oss << ",";
  }
  oss << "]}";
  return oss.str();
}

} // namespace wsce::net