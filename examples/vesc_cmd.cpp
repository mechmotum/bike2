#include "third_party/vesc_uart/VescUart.h"
#include "third_party/vesc_uart/extra.hpp"

#include <chrono>
#include <cstdint>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <thread>

auto main() -> int
{
  const auto current = -10.0F;

  while (true) {
    ::VescUartSetCurrent(current);
    fmt::print("current: {}\n", std::int16_t(current));

    const auto start = std::chrono::steady_clock::now();
    fmt::print("rpm: {}\n", ::vesc::extra::read_rpm());

    const auto elapsed = std::chrono::steady_clock::now() - start;
    fmt::print("{}\n", std::chrono::duration<float>{elapsed}.count());

    // NOLINTNEXTLINE(readability-magic-numbers)
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
  }

  return 0;
}
