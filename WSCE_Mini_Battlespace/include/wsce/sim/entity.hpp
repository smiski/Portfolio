#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "wsce/sim/integrator.hpp"

namespace wsce::sim {

class World; // forward

// Base class for all simulated entities (aircraft, missile, etc.)
class Entity {
public:
  Entity(std::uint64_t id, std::string name) : id_(id), name_(std::move(name)) {}
  virtual ~Entity() = default;

  std::uint64_t id() const { return id_; }
  const std::string& name() const { return name_; }

  const KinematicState& state() const { return state_; }
  KinematicState& state_mut() { return state_; }

  virtual void tick(World& world, double dt) = 0;

protected:
  std::uint64_t id_;
  std::string name_;
  KinematicState state_;
};

using EntityPtr = std::shared_ptr<Entity>;

} // namespace wsce::sim