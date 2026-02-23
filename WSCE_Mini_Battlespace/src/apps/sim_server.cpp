#include "wsce/config.hpp"
#include "wsce/util/log.hpp"
#include "wsce/util/time.hpp"

#include "wsce/core/thread_pool.hpp"
#include "wsce/net/udp_socket.hpp"
#include "wsce/net/telemetry.hpp"
#include "wsce/net/commands.hpp"

#include "wsce/sim/world.hpp"
#include "wsce/sim/aircraft.hpp"
#include "wsce/sim/missile.hpp"
#include "wsce/sim/integrator.hpp"

#include <atomic>
#include <sstream>
#include <thread>

using namespace wsce;

static std::size_t choose_threads() {
  if (config::kWorkerThreads > 0) return (std::size_t)config::kWorkerThreads;
  auto hc = std::thread::hardware_concurrency();
  return (hc == 0) ? 4 : (std::size_t)hc;
}

int main() {
  log::info("Starting WSCE Mini Battlespace Simulation Server...");

  sim::World world(choose_threads());

  // Create a target aircraft
  auto tgtId = world.next_id();
  auto aircraft = std::make_shared<sim::Aircraft>(tgtId, "aircraft_red",
    std::make_unique<sim::RK4Integrator>());

  aircraft->state_mut().pos = {0, 0, 1000};
  aircraft->state_mut().vel = {250, 0, 0};
  world.add(aircraft);

  // Create a "blue" launcher aircraft (not strictly needed, but shows multi-entity).
  auto blueId = world.next_id();
  auto blue = std::make_shared<sim::Aircraft>(blueId, "aircraft_blue",
    std::make_unique<sim::EulerIntegrator>());
  blue->state_mut().pos = {-5000, -2000, 1200};
  blue->state_mut().vel = {260, 20, 0};
  world.add(blue);

  net::UdpSocket telem;
  telem.set_broadcast(false);

  net::UdpSocket cmdSock;
  cmdSock.bind(config::kCommandPort);

  double simTime = 0.0;
  const auto wallStart = time::Clock::now();

  bool missileSpawned = false;

  while (simTime < config::kMaxSimSec) {
    // --- command/control (non-blocking-ish) ---
    if (auto line = cmdSock.recv_from(1); !line.empty()) {
      auto cmd = net::parse_command(line);
      if (cmd) {
        if (cmd->type == net::CommandType::SetTargetVelocity) {
          aircraft->set_target_velocity(cmd->vec);
          log::info("Command: set_target_vel " + cmd->vec.str());
        } else if (cmd->type == net::CommandType::SpawnMissile) {
          auto mid = world.next_id();
          auto m = std::make_shared<sim::Missile>(mid, "missile_blue",
            std::make_unique<sim::RK4Integrator>(), tgtId);

          // Spawn near blue aircraft
          m->state_mut().pos = blue->state().pos;
          m->state_mut().vel = blue->state().vel + math::Vec3{200, 0, 0};
          world.add(m);
          missileSpawned = true;

          log::info("Command: spawn_missile (id=" + std::to_string(mid) + ")");
        } else if (cmd->type == net::CommandType::Reset) {
          log::warn("Command: reset (demo reset only adjusts target position)");
          aircraft->state_mut().pos = {0, 0, 1000};
          aircraft->state_mut().vel = {250, 0, 0};
          missileSpawned = false;
        }
      }
    }

    // Auto-spawn a missile after a few seconds for the demo.
    if (!missileSpawned && simTime > 3.0) {
      auto mid = world.next_id();
      auto m = std::make_shared<sim::Missile>(mid, "missile_blue",
        std::make_unique<sim::RK4Integrator>(), tgtId);
      m->state_mut().pos = blue->state().pos;
      m->state_mut().vel = blue->state().vel + math::Vec3{200, 0, 0};
      world.add(m);
      missileSpawned = true;
      log::info("Auto: spawned missile.");
    }

    // --- simulation step (parallel) ---
    world.step(config::kFixedDtSec);
    simTime += config::kFixedDtSec;

    // --- telemetry publish ---
    const auto snap = world.snapshot();
    const auto msg = net::encode_snapshot_json(snap, simTime);
    telem.send_to("127.0.0.1", config::kTelemetryPort, msg);

    // --- real-time pacing ---
    const double wall = time::seconds_since(wallStart);
    const double desired = simTime;
    const double lag = desired - wall;
    if (lag > 0.0) {
      std::this_thread::sleep_for(std::chrono::duration<double>(std::min(lag, 0.01)));
    }
  }

  log::info("Server exiting after demo runtime.");
  return 0;
}