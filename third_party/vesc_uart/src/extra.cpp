#include "third_party/vesc_uart/extra.hpp"

#include "third_party/vesc_uart/VescUart.h"

#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstring>

namespace {

static constexpr auto serial_device = 0;

auto payload_buffer = std::array<std::uint8_t, 256>{};

auto cmd_get_values = std::uint8_t{COMM_GET_VALUES};

}  // namespace

namespace vesc::extra {

// https://github.com/vedderb/bldc/blob/master/comm/commands.c#L379

auto read_payload(std::span<std::uint8_t> buf) -> void
{
  ::PackSendPayload(&cmd_get_values, sizeof(cmd_get_values), serial_device);

  {
    [[maybe_unused]] const auto version = Serial.read();
    assert(version == 2 and "only version 2 is handled");
  }

  const auto payload_size = Serial.read();
  static constexpr auto footer_size = 3;

  assert(
      buf.size() >= payload_size and
      "`buf` is not large enough to hold "
      "message");

  const auto message =
      std::span{buf.data(), std::size_t(payload_size + footer_size)};
  Serial.read(message);
}

auto read_rpm() -> std::int32_t
{
  read_payload(payload_buffer);

  // this is assumed by examining payload bytes
  const auto rpm_bytes = std::span{payload_buffer.data() + 23, 4};

  auto rpm = std::int32_t{};
  std::memcpy(&rpm, rpm_bytes.data(), rpm_bytes.size());

  return std::byteswap(rpm);
}

}  // namespace vesc::extra
