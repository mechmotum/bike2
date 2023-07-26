extern "C" {
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
}

#include <cassert>
#include <fcntl.h>
#include <fmt/core.h>
#include <sys/ioctl.h>

// https://docs.kernel.org/i2c/dev-interface.html

auto main(int argc, char** argv) -> int
{
  assert(
      false and
      "This example is only intended to test building and linking "
      "with libi2c");

  if (argc < 2) {
    fmt::print("./i2c <device>");
    return 1;
  }

  const auto fd = ::open(argv[1], O_RDWR);
  if (fd < 0) {
    fmt::print("error in opening device");
    return 1;
  }

  constexpr auto addr = int{0x40}; /* The I2C address */

  if (::ioctl(fd, I2C_SLAVE, addr) < 0) {
    fmt::print("error specifying i2c device address");
    return 1;
  }

  ::i2c_smbus_read_block_data(fd, 0, nullptr);
  return 0;
}
