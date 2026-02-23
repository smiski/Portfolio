#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace wsce::net {

// Cross-platform UDP socket wrapper (RAII).
class UdpSocket {
public:
  UdpSocket();
  ~UdpSocket();

  UdpSocket(const UdpSocket&) = delete;
  UdpSocket& operator=(const UdpSocket&) = delete;

  void bind(uint16_t port);
  void set_broadcast(bool enabled);

  void send_to(const std::string& host, uint16_t port, const std::string& msg);

  // Non-blocking receive with timeout (ms). Returns empty string on timeout.
  std::string recv_from(uint16_t timeoutMs, std::string* outFromHost = nullptr, uint16_t* outFromPort = nullptr);

private:
  int sock_{-1};
  bool valid_{false};
};

} // namespace wsce::net