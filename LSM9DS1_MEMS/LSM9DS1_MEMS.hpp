#pragma once

extern "C" {
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>
}

#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fcntl.h>
#include <sys/ioctl.h>

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
  using scalar_type = float;

  LSM9DS1(const char* device)
  {
    // NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer,
    // cppcoreguidelines-pro-type-vararg)
    device_ = ::open(device, O_RDWR);
    // NOLINTEND(cppcoreguidelines-prefer-member-initializer,
    // cppcoreguidelines-pro-type-vararg)
    assert((device_ >= 0) and "error in opening device");

    static constexpr auto addr = 0x6B;

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
    [[maybe_unused]] const auto set_addr_status =
        ::ioctl(device_, I2C_SLAVE, addr);
    assert((set_addr_status >= 0) and "error specifying i2c device address");

    init_gyro();
    init_accel();
  }

  auto read_gx() const -> std::int16_t
  {
    auto raw = i2c_read(OUT_X_L_G);
    return raw;
  }

  struct Accel
  {
    std::int16_t y;
    std::int16_t z;

    [[nodiscard]]
    auto get_theta() const -> scalar_type
    {
      return std::atan2(scalar_type(z), scalar_type(y));
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

  auto i2c_write(std::uint8_t subaddress, std::uint8_t data) const -> void
  {
    [[maybe_unused]] const auto status =
        ::i2c_smbus_write_byte_data(device_, subaddress, data);
    assert((status == 0) and "Failed to write data");
  }

  [[nodiscard]]
  auto i2c_read(std::uint8_t subaddress) const -> std::int16_t
  {
    auto response = ::i2c_smbus_read_word_data(device_, subaddress);
    assert((response >= 0) and "Failed to read data");

    // NOLINTNEXTLINE(readability-magic-numbers)
    auto as_s16 = std::int16_t(response & 0xFFFF);

    return as_s16;
  }

  auto init_gyro() -> void
  {
    // NOLINTBEGIN(readability-magic-numbers)

    // CTRL_REG1_G
    {
      static constexpr std::uint8_t ODR_60Hz_LPF1_19Hz = 0b010 << 5;
      static constexpr std::uint8_t FS_245dps = 0b00 << 3;
      static constexpr std::uint8_t BW_00 = 0b00;
      i2c_write(CTRL_REG1_G, ODR_60Hz_LPF1_19Hz | FS_245dps | BW_00);
    }

    // CTRL_REG2_G
    {
      static constexpr std::uint8_t INT_SEL = 0b00 << 2;
      static constexpr std::uint8_t OUT_SEL = 0b00;
      i2c_write(CTRL_REG2_G, INT_SEL | OUT_SEL);
    }

    // CTRL_REG3_G
    {
      static constexpr std::uint8_t LP_disabled = 0b0 << 7;
      static constexpr std::uint8_t HPF_disabled = 0b0 << 6;
      static constexpr std::uint8_t HPCF_G_0000 = 0b0000;
      i2c_write(CTRL_REG3_G, LP_disabled | HPF_disabled | HPCF_G_0000);
    }

    // CTRL_REG4
    {
      static constexpr std::uint8_t Z_disable = 0b0 << 5;
      static constexpr std::uint8_t Y_disable = 0b0 << 4;
      static constexpr std::uint8_t X_enable = 0b1 << 3;
      static constexpr std::uint8_t LatchedInterrupt_enable = 0b1 << 1;

      i2c_write(
          CTRL_REG4,
          Z_disable | Y_disable | X_enable | LatchedInterrupt_enable);
    }

    // NOLINTEND(readability-magic-numbers)
  }

  auto init_accel() -> void
  {
    // NOLINTBEGIN(readability-magic-numbers)

    // CTRL_REG5_XL
    {
      static constexpr std::uint8_t DEC_none = 0b00 << 6;
      static constexpr std::uint8_t Z_enable = 0b1 << 5;
      static constexpr std::uint8_t Y_enable = 0b1 << 4;
      static constexpr std::uint8_t X_disable = 0b0 << 3;
      i2c_write(CTRL_REG5_XL, DEC_none | Z_enable | Y_enable | X_disable);
    }

    // CTRL_REG6_XL
    {
      static constexpr std::uint8_t ODR_952Hz = 0b110 << 5;
      static constexpr std::uint8_t FS_2g = 0b00 << 3;
      static constexpr std::uint8_t BW_odr = 0b0 << 2;
      static constexpr std::uint8_t BW_408Hz = 0b00;
      i2c_write(CTRL_REG6_XL, ODR_952Hz | FS_2g | BW_odr | BW_408Hz);
    }

    // CTRL_REG7_XL
    {
      static constexpr std::uint8_t HR_disable = 0b0 << 7;
      static constexpr std::uint8_t LPF_ODR_div_50 = 0b00 << 5;
      static constexpr std::uint8_t LPF_disable = 0b0 << 2;
      static constexpr std::uint8_t HP_int_disable = 0b0;
      i2c_write(
          CTRL_REG7_XL,
          HR_disable | LPF_ODR_div_50 | LPF_disable | HP_int_disable);
    }

    // CTRL_REG8
    {
      static constexpr std::uint8_t BLE_little = 0b1 << 1;
      i2c_write(CTRL_REG8, BLE_little);
    }

    // NOLINTEND(readability-magic-numbers)
  }
};
