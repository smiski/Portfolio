cmake_minimum_required(VERSION 3.16)
project(wsce_mini_battlespace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build unit tests" ON)

add_library(wsce
  src/util/log.cpp
  src/sim/aircraft.cpp
  src/sim/missile.cpp
  src/sim/world.cpp
  src/net/udp_socket.cpp
  src/net/telemetry.cpp
  src/net/commands.cpp
  src/core/thread_pool.cpp
)

target_include_directories(wsce PUBLIC include)

if (WIN32)
  target_compile_definitions(wsce PUBLIC WSCE_PLATFORM_WINDOWS)
  target_link_libraries(wsce PUBLIC ws2_32)
else()
  target_compile_definitions(wsce PUBLIC WSCE_PLATFORM_POSIX)
endif()

add_executable(sim_server src/apps/sim_server.cpp)
target_link_libraries(sim_server PRIVATE wsce)

add_executable(sim_client src/apps/sim_client.cpp)
target_link_libraries(sim_client PRIVATE wsce)

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()