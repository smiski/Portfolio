#pragma once
#include <memory>
#include "wsce/sim/entity.hpp"
#include "wsce/sim/integrator.hpp"

namespace wsce::sim {

// Missile uses Proportional Navigation (PN) guidance.
// PN is a classic guidance law used in real missile systems and in many simulators.
class Missile final : public Entity {
public:
  Missile(std::uint64_t id, std::string name, std::unique_ptr<Integrator> integ, std::uint64_t targetId);

  void tick(World& world, double dt) override;

  bool is_dead() const { return dead_; }

private:
  std::unique_ptr<Integrator> integ_;
  std::uint64_t targetId_{0};

  double navConstant_{3.0};     // N (typical ~3-5)
  double maxAccel_{60.0};       // m/s^2
  double detonateRange_{25.0};  // meters

  bool dead_{false};
};

} // namespace wsce::sim