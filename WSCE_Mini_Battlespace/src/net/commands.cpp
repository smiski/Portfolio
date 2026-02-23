#include "wsce/net/commands.hpp"
#include <sstream>

namespace wsce::net {

std::optional<Command> parse_command(const std::string& line) {
  std::istringstream iss(line);
  std::string op;
  iss >> op;
  if (op.empty()) return std::nullopt;

  if (op == "set_target_vel") {
    double x=0, y=0, z=0;
    if (!(iss >> x >> y >> z)) return std::nullopt;
    return Command{CommandType::SetTargetVelocity, wsce::math::Vec3{x,y,z}};
  }
  if (op == "spawn_missile") {
    return Command{CommandType::SpawnMissile, {}};
  }
  if (op == "reset") {
    return Command{CommandType::Reset, {}};
  }
  return Command{CommandType::Unknown, {}};
}

} // namespace wsce::net