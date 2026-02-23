#include "wsce/sim/world.hpp"
#include "wsce/util/log.hpp"
#include "wsce/sim/missile.hpp"
#include <algorithm>

namespace wsce::sim {

World::World(std::size_t workerThreads)
: pool_(workerThreads) {}

void World::add(const EntityPtr& e) {
  std::lock_guard<std::mutex> lk(m_);
  entities_[e->id()] = e;
}

EntityPtr World::get(std::uint64_t id) const {
  std::lock_guard<std::mutex> lk(m_);
  auto it = entities_.find(id);
  return (it == entities_.end()) ? nullptr : it->second;
}

void World::step(double dt) {
  // Copy pointers for ticking without holding the map lock.
  std::vector<EntityPtr> ents;
  {
    std::lock_guard<std::mutex> lk(m_);
    ents.reserve(entities_.size());
    for (auto& kv : entities_) ents.push_back(kv.second);
  }

  // Parallel tick: each entity is updated independently.
  for (auto& e : ents) {
    pool_.enqueue([this, e, dt] {
      e->tick(*this, dt);
    });
  }
  pool_.wait_idle();

  // Cleanup dead missiles.
  {
    std::lock_guard<std::mutex> lk(m_);
    for (auto it = entities_.begin(); it != entities_.end(); ) {
      if (auto msl = std::dynamic_pointer_cast<wsce::sim::Missile>(it->second)) {
        if (msl->is_dead()) { it = entities_.erase(it); continue; }
      }
      ++it;
    }
  }
}

std::vector<World::SnapshotItem> World::snapshot() const {
  std::vector<SnapshotItem> out;
  std::lock_guard<std::mutex> lk(m_);
  out.reserve(entities_.size());
  for (const auto& kv : entities_) {
    out.push_back(SnapshotItem{kv.second->id(), kv.second->name(), kv.second->state()});
  }
  return out;
}

} // namespace wsce::sim