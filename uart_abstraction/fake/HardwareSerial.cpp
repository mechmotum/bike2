#include "uart_abstraction/HardwareSerial.hpp"

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto HardwareSerial::available() const -> bool
{
  return true;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto HardwareSerial::read() const -> std::uint8_t
{
  return {};
}

auto HardwareSerial::write(
    [[maybe_unused]] std::uint8_t* buffer,
    [[maybe_unused]] std::size_t size) const -> void
{}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
HardwareSerial Serial;
