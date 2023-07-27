#pragma once

#include <cstdint>
#include <span>

namespace vesc::extra {

auto read_payload(std::span<std::uint8_t> buf) -> void;

auto read_rpm() -> std::int32_t;

}  // namespace vesc::extra
