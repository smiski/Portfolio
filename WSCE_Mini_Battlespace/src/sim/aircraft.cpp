#include "wsce/sim/aircraft.hpp"
#include "wsce/sim/world.hpp"
#include "wsce/util/log.hpp"
#include <algorithm>

namespace wsce::sim {

Aircraft::Aircraft(std::uint64_t id, std::string name, std::unique_ptr<Integrator> integ)
: Entity(id, std::move(name)), integ_(std::move(integ)) {}

void Aircraft::tick(World&, double dt) {
  // Acceleration = clamp( (targetVel - vel)/tau ) with maxAccel cap.
  // This is a simple "velocity hold" autopilot model.
  const double tau = 1.5; // seconds (response time constant)

  integ_->step(state_, dt, [&](const KinematicState& s) {
    wsce::math::Vec3 desiredAcc = (targetVel_ - s.vel) / tau;
    const double mag = desiredAcc.norm();
    if (mag > maxAccel_) desiredAcc = desiredAcc * (maxAccel_ / mag);
    return desiredAcc;
  });
}

} // namespace wsce::sim