#include "third_party/vesc_uart/VescUart.h"
//#include ""

#include <cstdint>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <chrono>
#include <thread>
#include <array>
#include <span>
#include <cstring>
#include <bit>

auto buffer = std::array<std::uint8_t, 256>{};

auto read_payload(std::span<std::uint8_t> buf) -> void
{
  static constexpr auto serial_device = 0;

  static auto cmd_get_values = std::uint8_t{ COMM_GET_VALUES };
  ::PackSendPayload(&cmd_get_values, sizeof(cmd_get_values), serial_device);

  {
    const auto version = Serial.read();
    assert(version == 2 and "only version 2 is handled");
  }

  const auto payload_size = Serial.read();
  static constexpr auto footer_size = 3;

  fmt::print("payload_size: {}\n", payload_size);

  const auto message = std::span{buf.data(), std::size_t(payload_size + footer_size)};
  Serial.read(message);

  const auto front = std::span{buf.data(),
                               //2 + 2 + 4 + 4 + 4 + 4 + 2 + 4
                               30
  };
  fmt::print("{::0<2X}\n", front);

  const auto rpm_buf = std::span{buf.data() + 23,
                               //2 + 2 + 4 + 4 + 4 + 4 + 2 + 4
                               4};
  fmt::print("{::0<2X}\n", rpm_buf);

  auto rpm = std::int32_t{};
  std::memcpy(&rpm, rpm_buf.data(), rpm_buf.size());

  fmt::print("{}\n", std::byteswap(rpm));

  // https://github.com/vedderb/bldc/blob/master/comm/commands.c#L379
}

auto main() -> int
{
  const auto current = -10.0F;
  //const auto data = std::span{reinterpret_cast<const std::uint8_t*>(&current), sizeof(current)};

  while (true) {
    //::VescUartSetCurrent(current += 1.0F);
    ::VescUartSetCurrent(current);
    fmt::print("current: {}\n", std::int16_t(current));

    read_payload({buffer.data(), buffer.size()});

    std::this_thread::sleep_for(std::chrono::milliseconds{100});
  }

  return 0;
}
