#include "wsce/sim/world.hpp"
#include "wsce/sim/aircraft.hpp"
#include "wsce/sim/missile.hpp"
#include "wsce/sim/integrator.hpp"
#include <cassert>

int main() {
  wsce::sim::World w(2);

  auto tgtId = w.next_id();
  auto tgt = std::make_shared<wsce::sim::Aircraft>(tgtId, "tgt",
    std::make_unique<wsce::sim::EulerIntegrator>());
  tgt->state_mut().pos = {0,0,1000};
  tgt->state_mut().vel = {200,0,0};
  w.add(tgt);

  auto mid = w.next_id();
  auto m = std::make_shared<wsce::sim::Missile>(mid, "m",
    std::make_unique<wsce::sim::RK4Integrator>(), tgtId);
  m->state_mut().pos = {-2000,0,1000};
  m->state_mut().vel = {300,0,0};
  w.add(m);

  double lastRange = (tgt->state().pos - m->state().pos).norm();
  for (int i = 0; i < 200; ++i) {
    w.step(0.02);
    double r = (tgt->state().pos - m->state().pos).norm();
    // Range should generally decrease early on in this geometry.
    if (i < 50) assert(r < lastRange + 1e-6);
    lastRange = r;
  }
  return 0;
}