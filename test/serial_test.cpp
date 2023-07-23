#include <asio.hpp>
#include <cstdlib>
#include <fmt/core.h>
#include <stdexcept>
#include <string_view>

auto main(int argc, const char** argv) -> int
{
  if (argc < 2) {
    throw std::runtime_error{"device path not specified"};
  }

  auto ctx = asio::io_context{};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  auto serial = asio::serial_port{ctx, argv[1]};
  {
    using opt = asio::serial_port_base;

    // NOLINTBEGIN(readability-magic-numbers)
    serial.set_option(opt::baud_rate{115200});
    serial.set_option(opt::character_size{8});
    serial.set_option(opt::parity{opt::parity::none});
    serial.set_option(opt::stop_bits{opt::stop_bits::one});
    // NOLINTEND(readability-magic-numbers)
  }

  constexpr auto sv = std::string_view{"hello, world!\n"};

  for (auto buf = asio::buffer(sv.data(), sv.size()); buf.size() != 0;) {
    buf += serial.write_some(asio::buffer(sv.data(), sv.size()));
  }

  return 0;
}
