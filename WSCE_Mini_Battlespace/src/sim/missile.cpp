#include "wsce/sim/missile.hpp"
#include "wsce/sim/world.hpp"
#include "wsce/util/log.hpp"
#include <sstream>

namespace wsce::sim {

Missile::Missile(std::uint64_t id, std::string name, std::unique_ptr<Integrator> integ, std::uint64_t targetId)
: Entity(id, std::move(name)), integ_(std::move(integ)), targetId_(targetId) {}

void Missile::tick(World& world, double dt) {
  if (dead_) return;

  auto tgt = world.get(targetId_);
  if (!tgt) {
    dead_ = true;
    wsce::log::warn("Missile lost target; self-terminating.");
    return;
  }

  const auto& ts = tgt->state();
  const auto& ms = state_;

  const wsce::math::Vec3 relPos = ts.pos - ms.pos;
  const wsce::math::Vec3 relVel = ts.vel - ms.vel;

  const double range = relPos.norm();
  if (range < detonateRange_) {
    dead_ = true;
    wsce::log::info("Missile detonated (range threshold met).");
    return;
  }

  // --- Proportional Navigation (PN) guidance (point-mass approximation) ---
  // LOS rate omega ≈ (r x v) / |r|^2
  // Commanded lateral acceleration: a_cmd = N * Vc * (omega x r_hat)
  // where closing speed Vc = -dot(r_hat, relVel)
  const auto rhat = relPos.normalized();
  const double closingSpeed = -rhat.dot(relVel); // positive if closing
  const auto omega = relPos.cross(relVel) / (range*range + 1e-9);
  wsce::math::Vec3 a_cmd = (navConstant_ * closingSpeed) * (omega.cross(rhat));

  // Cap acceleration (represents missile maneuver limit)
  const double a_mag = a_cmd.norm();
  if (a_mag > maxAccel_) a_cmd = a_cmd * (maxAccel_ / a_mag);

  integ_->step(state_, dt, [&](const KinematicState&) {
    // Optional gravity could be added here; kept zero for clarity.
    return a_cmd;
  });
}

} // namespace wsce::sim