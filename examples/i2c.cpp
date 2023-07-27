#include "LSM9DS1_MEMS/LSM9DS1_MEMS.hpp"

#include <chrono>
#include <fmt/core.h>
#include <thread>

auto main(int argc, char** argv) -> int
{
  if (argc < 2) {
    fmt::print("./i2c <device>");
    return 1;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const auto& device = argv[1];
  constexpr auto i2c_address = 0x6b;
  auto sensor = LSM9DS1{device, i2c_address};

  // NOLINTNEXTLINE(readability-magic-numbers)
  for (auto i = 0; i < 100; ++i) {
    (void)sensor.read_gx();
    (void)sensor.read_accel();

    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }

  while (true) {
    auto gx = sensor.read_gx();
    fmt::print("gx raw: {}\n", gx);

    auto accel = sensor.read_accel();
    fmt::print(
        "accel y raw: {}, accel z raw: {}, theta: {}\n",
        accel.y,
        accel.z,
        accel.get_theta());

    constexpr auto delay = std::chrono::milliseconds{100};
    std::this_thread::sleep_for(delay);
  }

  return 0;
}
