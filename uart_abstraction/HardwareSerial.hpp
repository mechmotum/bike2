// API for UART read/write.

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

struct HardwareSerial
{
  HardwareSerial(const char* dev);

  [[nodiscard]]
  auto available() const -> bool;

  auto read() const -> std::uint8_t;

  auto read(std::span<std::uint8_t> buffer) const -> void;

  auto write(std::uint8_t* buffer, std::size_t size) const -> void;
};

extern HardwareSerial Serial;
