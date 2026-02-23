#pragma once
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "wsce/core/thread_pool.hpp"
#include "wsce/sim/entity.hpp"

namespace wsce::sim {

class World {
public:
  explicit World(std::size_t workerThreads);

  void add(const EntityPtr& e);
  EntityPtr get(std::uint64_t id) const;

  // Core simulation step (parallel entity tick).
  void step(double dt);

  // Snapshot for telemetry (copy states in a consistent way).
  struct SnapshotItem {
    std::uint64_t id;
    std::string name;
    KinematicState state;
  };
  std::vector<SnapshotItem> snapshot() const;

  std::uint64_t next_id() { return ++idGen_; }

private:
  mutable std::mutex m_;
  std::unordered_map<std::uint64_t, EntityPtr> entities_;

  wsce::core::ThreadPool pool_;
  std::atomic<std::uint64_t> idGen_{1000};
};

} // namespace wsce::sim