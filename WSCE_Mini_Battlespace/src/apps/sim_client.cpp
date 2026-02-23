#include "wsce/config.hpp"
#include "wsce/net/udp_socket.hpp"
#include "wsce/util/log.hpp"
#include <iostream>

using namespace wsce;

int main() {
  log::info("Starting WSCE Mini Battlespace Telemetry Client...");
  net::UdpSocket sock;
  sock.bind(config::kTelemetryPort);

  for (;;) {
    auto msg = sock.recv_from(250);
    if (msg.empty()) continue;

    std::cout << msg << "\n";
  }
}