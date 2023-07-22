// Compatibility header to make the library build outside of Arduino projects.

#ifndef WPROGAM_H_
#define WPROGAM_H_

// For `size_t`
#include <cstddef>

// For `memcpy`
#include <cstring>

// Some strange convention in the library, probable stemming from the Arduino
// world.
using boolean = bool;

// Somewhat awkward sleep_for wrapper
#include <cassert>
#include <chrono>
#include <thread>
auto delay(int milliseconds) -> void
{
  assert((milliseconds >= 0) && "A negative sleep duration requested");

  std::this_thread::sleep_for(std::chrono::milliseconds{milliseconds});
}

// The library doesn't perform actual harware UART interaction, but needs an
// abstraction.
#include "uart_abstraction/HardwareSerial.hpp"

#endif
