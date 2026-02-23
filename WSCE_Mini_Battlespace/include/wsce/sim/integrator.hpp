#pragma once
#include <functional>
#include "wsce/math/vec3.hpp"

namespace wsce::sim {

// State for simple point-mass kinematics (position, velocity).
struct KinematicState {
  wsce::math::Vec3 pos;
  wsce::math::Vec3 vel;
};

// Strategy Pattern: swap integrators without touching entity logic.
class Integrator {
public:
  virtual ~Integrator() = default;

  // acceleration_fn returns acceleration given the current state
  virtual void step(KinematicState& s, double dt,
                    const std::function<wsce::math::Vec3(const KinematicState&)>& acceleration_fn) = 0;
};

class EulerIntegrator final : public Integrator {
public:
  void step(KinematicState& s, double dt,
            const std::function<wsce::math::Vec3(const KinematicState&)>& a) override {
    const auto acc = a(s);
    s.pos += s.vel * dt;
    s.vel += acc * dt;
  }
};

// RK4 for higher fidelity (common in simulation).
class RK4Integrator final : public Integrator {
public:
  void step(KinematicState& s, double dt,
            const std::function<wsce::math::Vec3(const KinematicState&)>& a) override {
    // For point-mass kinematics:
    // dpos/dt = vel, dvel/dt = acc(state)
    auto eval = [&](const KinematicState& st) {
      return std::pair<wsce::math::Vec3, wsce::math::Vec3>{st.vel, a(st)};
    };

    const auto [k1p, k1v] = eval(s);

    KinematicState s2 = s; s2.pos += k1p*(dt*0.5); s2.vel += k1v*(dt*0.5);
    const auto [k2p, k2v] = eval(s2);

    KinematicState s3 = s; s3.pos += k2p*(dt*0.5); s3.vel += k2v*(dt*0.5);
    const auto [k3p, k3v] = eval(s3);

    KinematicState s4 = s; s4.pos += k3p*dt; s4.vel += k3v*dt;
    const auto [k4p, k4v] = eval(s4);

    s.pos += (k1p + 2.0*k2p + 2.0*k3p + k4p) * (dt/6.0);
    s.vel += (k1v + 2.0*k2v + 2.0*k3v + k4v) * (dt/6.0);
  }
};

} // namespace wsce::sim