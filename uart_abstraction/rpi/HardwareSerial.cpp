#include "uart_abstraction/HardwareSerial.hpp"

#include <asio.hpp>
#include <cassert>
#include <optional>
#include <stdexcept>
#include <sys/select.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)

namespace {
auto ctx = asio::io_context{};
auto underlying = std::optional<asio::serial_port>{};
}  // namespace

HardwareSerial Serial{"/dev/ttyS0"};

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

HardwareSerial::HardwareSerial(const char* device)
{
  underlying.emplace(ctx, device);

  using opt = asio::serial_port_base;

  // NOLINTBEGIN(readability-magic-numbers)
  underlying->set_option(opt::baud_rate{115200});
  underlying->set_option(opt::character_size{8});
  underlying->set_option(opt::parity{opt::parity::none});
  underlying->set_option(opt::stop_bits{opt::stop_bits::one});
  // NOLINTEND(readability-magic-numbers)

  assert(underlying->is_open() and "underlying device not open");
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto HardwareSerial::available() const -> bool
{
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  auto native = underlying->native_handle();

  ::fd_set rfds;
  auto tv = ::timeval{{}, {}};

  FD_ZERO(&rfds);
  FD_SET(native, &rfds);

  const auto retval = ::select(1, &rfds, NULL, NULL, &tv);
  assert(retval >= 0 and "error in checking serial availability");

  return retval > 0;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto HardwareSerial::read() const -> std::uint8_t
{
  auto c = char{};

  for (auto n = std::size_t{}; n != 1;) {
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    n = underlying->read_some(asio::buffer(&c, 1));
    assert(n <= 1 and "read more than 1 byte");
  }

  return static_cast<std::uint8_t>(c);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
auto HardwareSerial::write(std::uint8_t* buffer, std::size_t size) const -> void
{
  for (auto buf = asio::buffer(buffer, size); buf.size() != 0;) {
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    buf += underlying->write_some(buf);
  }
}
