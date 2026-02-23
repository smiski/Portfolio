#pragma once
#include <memory>
#include "wsce/sim/entity.hpp"
#include "wsce/sim/integrator.hpp"

namespace wsce::sim {

// Simple aircraft model: target-velocity "autopilot" via acceleration limits.
// This is intentionally modest but realistic enough to discuss control logic.
class Aircraft final : public Entity {
public:
  Aircraft(std::uint64_t id, std::string name, std::unique_ptr<Integrator> integ);

  void set_target_velocity(const wsce::math::Vec3& v) { targetVel_ = v; }
  wsce::math::Vec3 target_velocity() const { return targetVel_; }

  void tick(World& world, double dt) override;

private:
  std::unique_ptr<Integrator> integ_;
  wsce::math::Vec3 targetVel_{250, 0, 0};  // m/s
  double maxAccel_{25.0};                  // m/s^2
};

} // namespace wsce::sim