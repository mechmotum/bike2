#include "LSM9DS1_MEMS/LSM9DS1_MEMS.hpp"
#include "filter/filter.hpp"
#include "third_party/vesc_uart/VescUart.h"
#include "third_party/vesc_uart/extra.hpp"

#include <chrono>
#include <fmt/core.h>
#include <iostream>
#include <limits>
#include <numbers>
#include <thread>

// Clang does not yet support floating point NTTP
//
template <class T>
struct constant
{
  using value_type = T;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const value_type value;

  consteval constant(value_type val) : value{val} {}

  constexpr operator T() const { return value; }
};

auto PD_controller(float theta, float omega, float wheel_rotation_speed)
    -> float
{
  constexpr auto P_angle = -1.5F * 1500.f;
  constexpr auto D_angle = -1.F * 400.f;

  constexpr auto scale_wheel = +1.F * 0.35F;

  constexpr auto D_wheel = +1.F * 1.9F;

  constexpr auto scale_all = 1.0F;

  return scale_all *
         (scale_wheel * (P_angle * theta + D_angle * omega) +
          D_wheel * wheel_rotation_speed);
}

// NOLINTBEGIN(readability-magic-numbers)
constexpr auto sample_period = constant{0.01F};  // s;
constexpr auto dt = std::chrono::milliseconds{10};
// // NOLINTEND(readability-magic-numbers)

auto main(int argc, char** argv) -> int
{

  if (argc < 2) {
    fmt::print("./i2c <device>");
    return 1;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  const auto& device = argv[1];
  auto sensor = LSM9DS1{device};

  float theta_g{};
  auto highpass_theta_g = filter::highpass<1, sample_period, float>{};
  auto lowpss_theta_a = filter::lowpass<1, sample_period, float>{};
  auto lowpss_omega_x = filter::lowpass<8, sample_period, float>{};
  auto lowpss_wheel_rot = filter::lowpass<5, sample_period, float>{};

  fmt::print("time;ay;az;wx;wm;theta_a;theta_g;theta_af;theta_gf;theta;I\n");

  while (true) {
    const auto now = std::chrono::steady_clock::now();

    const auto wheel_rpm = -float(::vesc::extra::read_rpm()) / 45.F;
    const auto wheel_rotation_speed =
        wheel_rpm / 60.F * 2.F * std::numbers::pi_v<float>;
    const auto wheel_rotation_speed_f =
        lowpss_wheel_rot(wheel_rotation_speed).value;

    const auto gx = sensor.read_gx();
    const auto omega_x_deg_s =
        gx * 245.f / std::numeric_limits<decltype(gx)>::max();
    const auto omega_x_rad_s =
        omega_x_deg_s * std::numbers::pi_v<float> / 180.f;
    const auto omega_xf = lowpss_omega_x(omega_x_rad_s).value;

    theta_g += std::chrono::duration<float>{dt}.count() * omega_x_rad_s;
    const auto theta_gf = highpass_theta_g(theta_g).value;

    const auto accel = sensor.read_accel();
    constexpr auto theta_a_calib = -0.971F / 180.F * std::numbers::pi_v<float>;
    const auto theta_a = accel.get_theta() + theta_a_calib;
    const auto theta_af = lowpss_theta_a(theta_a).value;

    const auto theta = theta_af + theta_gf;
    const auto current_request =
        PD_controller(theta, omega_xf, wheel_rotation_speed_f);
    ::VescUartSetCurrent(current_request);

    const auto deadline = now + dt;

    fmt::print(
        "{:+f};{};{};{:+f};{:+f};{:+f};{:+f};{:+f};{:+f};{:+f};{:+f}\n",
        std::chrono::duration<float>{now.time_since_epoch()}.count(),
        accel.y,
        accel.z,
        omega_x_rad_s,
        wheel_rotation_speed_f,
        theta_a,
        theta_g,
        theta_af,
        theta_gf,
        theta,
        current_request);

    std::cerr
        << "Sleep time: "
        << 1000.f *
               std::chrono::duration<float>{
                   deadline - std::chrono::steady_clock::now()}
                   .count()
        << std::endl;

    std::this_thread::sleep_until(deadline);
  }

  return 0;
}
