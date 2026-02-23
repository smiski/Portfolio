#include "wsce/net/udp_socket.hpp"
#include "wsce/util/log.hpp"

#include <cstring>

#if defined(WSCE_PLATFORM_WINDOWS)
  #include <winsock2.h>
  #include <ws2tcpip.h>
  static bool g_wsa_init = false;
#else
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <unistd.h>
  #include <fcntl.h>
#endif

namespace wsce::net {

static void platform_init() {
#if defined(WSCE_PLATFORM_WINDOWS)
  if (!g_wsa_init) {
    WSADATA wsa{};
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
      wsce::log::error("WSAStartup failed.");
    } else {
      g_wsa_init = true;
    }
  }
#endif
}

static void platform_close(int s) {
#if defined(WSCE_PLATFORM_WINDOWS)
  closesocket(s);
#else
  close(s);
#endif
}

UdpSocket::UdpSocket() {
  platform_init();
  sock_ = (int)::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  valid_ = (sock_ >= 0);

  if (!valid_) wsce::log::error("Failed to create UDP socket.");

#if !defined(WSCE_PLATFORM_WINDOWS)
  int flags = fcntl(sock_, F_GETFL, 0);
  fcntl(sock_, F_SETFL, flags | O_NONBLOCK);
#endif
}

UdpSocket::~UdpSocket() {
  if (valid_) platform_close(sock_);
}

void UdpSocket::bind(uint16_t port) {
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (::bind(sock_, (sockaddr*)&addr, sizeof(addr)) != 0) {
    wsce::log::error("UDP bind failed on port " + std::to_string(port));
  } else {
    wsce::log::info("UDP bound on port " + std::to_string(port));
  }
}

void UdpSocket::set_broadcast(bool enabled) {
  int opt = enabled ? 1 : 0;
  if (setsockopt(sock_, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt)) != 0) {
    wsce::log::warn("Failed to set SO_BROADCAST.");
  }
}

void UdpSocket::send_to(const std::string& host, uint16_t port, const std::string& msg) {
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

#if defined(WSCE_PLATFORM_WINDOWS)
  inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
#else
  inet_aton(host.c_str(), &addr.sin_addr);
#endif

  ::sendto(sock_, msg.c_str(), (int)msg.size(), 0, (sockaddr*)&addr, sizeof(addr));
}

std::string UdpSocket::recv_from(uint16_t timeoutMs, std::string* outFromHost, uint16_t* outFromPort) {
  // Set recv timeout
#if defined(WSCE_PLATFORM_WINDOWS)
  DWORD tv = timeoutMs;
  setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#else
  timeval tv{};
  tv.tv_sec = timeoutMs / 1000;
  tv.tv_usec = (timeoutMs % 1000) * 1000;
  setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif

  sockaddr_in from{};
#if defined(WSCE_PLATFORM_WINDOWS)
  int fromLen = sizeof(from);
#else
  socklen_t fromLen = sizeof(from);
#endif

  char buf[2048];
  const int n = (int)::recvfrom(sock_, buf, sizeof(buf)-1, 0, (sockaddr*)&from, &fromLen);
  if (n <= 0) return {};

  buf[n] = '\0';

  if (outFromHost) {
    char ip[64]{};
#if defined(WSCE_PLATFORM_WINDOWS)
    inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));
#else
    std::strncpy(ip, inet_ntoa(from.sin_addr), sizeof(ip)-1);
#endif
    *outFromHost = ip;
  }
  if (outFromPort) *outFromPort = ntohs(from.sin_port);

  return std::string(buf);
}

} // namespace wsce::net