#pragma once
#include <string>
#include <vector>
#include "wsce/sim/world.hpp"

namespace wsce::net {

// Telemetry encoder (kept simple; easily swapped to protobuf/flatbuffers later).
std::string encode_snapshot_json(const std::vector<wsce::sim::World::SnapshotItem>& items,
                                 double simTimeSec);

} // namespace wsce::net