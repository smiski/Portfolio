#pragma once
#include <optional>
#include <string>
#include "wsce/math/vec3.hpp"

namespace wsce::net {

// Minimal command set, text-based for interoperability.
enum class CommandType {
  SetTargetVelocity,
  SpawnMissile,
  Reset,
  Unknown
};

struct Command {
  CommandType type{CommandType::Unknown};
  wsce::math::Vec3 vec{};
};

std::optional<Command> parse_command(const std::string& line);

} // namespace wsce::net