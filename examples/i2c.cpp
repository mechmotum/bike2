extern "C" {
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
}

#include <bit>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fcntl.h>
#include <fmt/core.h>
#include <sys/ioctl.h>
#include <thread>

// https://docs.kernel.org/i2c/dev-interface.html

// NOLINTBEGIN(modernize-macro-to-enum, cppcoreguidelines-macro-usage)
#define ACT_THS 0x04
#define ACT_DUR 0x05
#define INT_GEN_CFG_XL 0x06
#define INT_GEN_THS_X_XL 0x07
#define INT_GEN_THS_Y_XL 0x08
#define INT_GEN_THS_Z_XL 0x09
#define INT_GEN_DUR_XL 0x0A
#define REFERENCE_G 0x0B
#define INT1_CTRL 0x0C
#define INT2_CTRL 0x0D
#define WHO_AM_I_XG 0x0F
#define CTRL_REG1_G 0x10
#define CTRL_REG2_G 0x11
#define CTRL_REG3_G 0x12
#define ORIENT_CFG_G 0x13
#define INT_GEN_SRC_G 0x14
#define OUT_TEMP_L 0x15
#define OUT_TEMP_H 0x16
#define STATUS_REG_0 0x17
#define OUT_X_L_G 0x18
#define OUT_X_H_G 0x19
#define OUT_Y_L_G 0x1A
#define OUT_Y_H_G 0x1B
#define OUT_Z_L_G 0x1C
#define OUT_Z_H_G 0x1D
#define CTRL_REG4 0x1E
#define CTRL_REG5_XL 0x1F
#define CTRL_REG6_XL 0x20
#define CTRL_REG7_XL 0x21
#define CTRL_REG8 0x22
#define CTRL_REG9 0x23
#define CTRL_REG10 0x24
#define INT_GEN_SRC_XL 0x26
#define STATUS_REG_1 0x27
#define OUT_X_L_XL 0x28
#define OUT_X_H_XL 0x29
#define OUT_Y_L_XL 0x2A
#define OUT_Y_H_XL 0x2B
#define OUT_Z_L_XL 0x2C
#define OUT_Z_H_XL 0x2D
#define FIFO_CTRL 0x2E
#define FIFO_SRC 0x2F
#define INT_GEN_CFG_G 0x30
#define INT_GEN_THS_XH_G 0x31
#define INT_GEN_THS_XL_G 0x32
#define INT_GEN_THS_YH_G 0x33
#define INT_GEN_THS_YL_G 0x34
#define INT_GEN_THS_ZH_G 0x35
#define INT_GEN_THS_ZL_G 0x36
#define INT_GEN_DUR_G 0x37
// NOLINTEND(modernize-macro-to-enum, cppcoreguidelines-macro-usage)

class LSM9DS1
{
public:
  LSM9DS1(const char* device, uint8_t address)
  {
    // NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer,
    // cppcoreguidelines-pro-type-vararg)
    device_ = ::open(device, O_RDWR);
    // NOLINTEND(cppcoreguidelines-prefer-member-initializer,
    // cppcoreguidelines-pro-type-vararg)
    assert((device_ >= 0) and "error in opening device");

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    auto set_addr_status = ::ioctl(device_, I2C_SLAVE, address);
    assert((set_addr_status >= 0) and "error specifying i2c device address");

    init_gyro();
    init_accel();
  }

  auto read_gx() -> int16_t
  {
    auto raw = i2c_read(OUT_X_L_G);
    return raw;
  }

  struct Accel
  {
    int16_t y;
    int16_t z;

    [[nodiscard]]
    auto get_theta() const -> float
    {
      // GCC cannot find `std::atan2f`
      return std::atan2(float(y), float(z));
    }
  };

  [[nodiscard]]
  auto read_accel() const -> Accel
  {
    auto y = i2c_read(OUT_Y_L_XL);
    auto z = i2c_read(OUT_Z_L_XL);

    return {y, z};
  }

private:
  int device_;

  auto i2c_write(uint8_t subaddress, uint8_t data) const -> void
  {
    auto status = ::i2c_smbus_write_byte_data(device_, subaddress, data);
    assert((status == 0) and "Failed to write data");
  }

  [[nodiscard]]
  auto i2c_read(uint8_t subaddress) const -> int16_t
  {
    auto response = ::i2c_smbus_read_word_data(device_, subaddress);
    assert((response >= 0) and "Failed to read data");

    // NOLINTNEXTLINE(readability-magic-numbers)
    auto as_s16 = int16_t(response & 0xFFFF);

    return as_s16;
  }

  auto init_gyro() -> void
  {
    uint8_t tempRegValue = 0;

    // CTRL_REG1_G
    tempRegValue = 0;
    // NOLINTBEGIN(readability-magic-numbers)
    tempRegValue |= (1 << 7);
    tempRegValue |= (1 << 6);
    // NOLINTEND(readability-magic-numbers)
    i2c_write(CTRL_REG1_G, tempRegValue);

    // CTRL_REG2_G
    tempRegValue = 0;
    i2c_write(CTRL_REG2_G, tempRegValue);

    // CTRL_REG3_G
    tempRegValue = 0;
    i2c_write(CTRL_REG3_G, tempRegValue);

    // CTRL_REG4
    tempRegValue = 0;
    tempRegValue |= (1 << 3);  // X axis
    tempRegValue |= (1 << 1);  // latch interrupt
    i2c_write(CTRL_REG4, tempRegValue);

    // CFG_G
    tempRegValue = 0;
    i2c_write(INT_GEN_CFG_G, tempRegValue);
  }

  auto init_accel() -> void
  {
    uint8_t tempRegValue = 0;

    // CTRL_REG5_XL
    tempRegValue = 0;
    // NOLINTBEGIN(readability-magic-numbers)
    tempRegValue |= (1 << 5);
    tempRegValue |= (1 << 4);
    // NOLINTEND(readability-magic-numbers)
    i2c_write(CTRL_REG5_XL, tempRegValue);

    // CTRL_REG6_XL
    tempRegValue = 0;
    // NOLINTBEGIN(readability-magic-numbers)
    tempRegValue |= (1 << 7);
    tempRegValue |= (1 << 6);
    // NOLINTEND(readability-magic-numbers)
    i2c_write(CTRL_REG6_XL, tempRegValue);

    // CTRL_REG7_XL
    tempRegValue = 0;
    i2c_write(CTRL_REG7_XL, tempRegValue);

    // CTRL_REG8
    tempRegValue = 0;
    tempRegValue |= (1 << 1);  // Endianness
    i2c_write(CTRL_REG8, tempRegValue);
  }
};

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
