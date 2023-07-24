#pragma once

#include <mp-units/systems/si/si.h>
#include <numbers>

// Low and high pass filters
// https://gitlab.com/mechmotum/row_filter/-/blob/master/row_filter/complementary.py

namespace filter {

namespace mp = mp_units;
namespace si = mp_units::si;

template <
    auto CutoffFrequency,
    auto SamplePeriod,
    class Rep = double,
    mp::Quantity Q = mp::quantity<mp::si::radian, Rep>>
struct lowpass
{
  using rep_type = Rep;

  static constexpr auto w0 = mp::quantity<si::radian / si::second, rep_type>{
      2 * std::numbers::pi_v<rep_type> * CutoffFrequency};
  static constexpr auto dt = mp::quantity<si::second, rep_type>{SamplePeriod};

  using value_type = Q;
  using derivative_type = decltype(std::declval<value_type>() / dt);

  struct signal
  {
    value_type value{};
    derivative_type derivative{};
  };

  value_type prev_unfiltered{};
  signal prev_filtered{};

  constexpr auto operator()(value_type value) -> signal
  {
    static constexpr auto squared = [](auto x) { return x * x; };

    static constexpr auto a = squared(w0);
    static constexpr auto b = std::numbers::sqrt2_v<rep_type> * w0;

    // Integrate the filter state equation using the midpoint Euler method with
    // time step h
    static constexpr auto h = dt;
    static constexpr auto h2 = squared(h);
    static constexpr auto denom = 4 * mp::one + 2 * h * b + h2 * a;

    static constexpr auto A = (4 * mp::one + 2 * h * b - h2 * a) / denom;
    static constexpr auto B = 4 * h / denom;
    static constexpr auto C = -4 * h * a / denom;
    static constexpr auto D = (4 * mp::one - 2 * h * b - h2 * a) / denom;
    static constexpr auto E = 2 * h2 * a / denom;
    static constexpr auto F = 4 * mp::one * h * a / denom;

    const auto x0 = value;
    const auto x1 = prev_unfiltered;
    const auto [y1, yd1] = prev_filtered;

    const auto y0 = A * y1 + B * yd1 + E * (x0 + x1) / 2;
    const auto yd0 = C * y1 + D * yd1 + F * (x0 + x1) / 2;

    prev_unfiltered = x0;
    return prev_filtered = signal{y0, yd0};
  }
};

template <
    auto CutoffFrequency,
    auto SamplePeriod,
    class Rep = double,
    mp::Quantity Q = mp::quantity<mp::si::radian, Rep>>
struct highpass
{
  using rep_type = Rep;

  static constexpr auto w0 = mp::quantity<si::radian / si::second, rep_type>{
      2 * std::numbers::pi_v<rep_type> * CutoffFrequency};
  static constexpr auto dt = mp::quantity<si::second, rep_type>{SamplePeriod};

  using value_type = Q;

  using state_type = std::tuple<
      mp::quantity<mp::si::second, rep_type>,
      mp::quantity<mp::square(mp::si::second), rep_type>>;

  value_type prev_unfiltered{};
  state_type prev_state{};

  struct return_type
  {
    value_type value{};
    state_type state{};
  };

  constexpr auto operator()(value_type value) -> return_type
  {
    static constexpr auto squared = [](auto x) { return x * x; };

    static constexpr auto h = dt;

    const auto xi = value;
    const auto xim1 = prev_unfiltered;
    const auto [z1im1, z2im1] = prev_state;

    static constexpr auto a0 = std::numbers::sqrt2_v<rep_type> * h * w0;
    static constexpr auto a1 = squared(h);
    static constexpr auto a2 = squared(w0);
    static constexpr auto a3 = a1 * a2;
    static constexpr auto a4 = 2 * a0;
    static constexpr auto a5 = a3 + a4 + 4 * mp::one;
    static constexpr auto a6 = 1 / a5;
    const auto a7 =
        a1 * xi + a1 * xim1 - a3 * z2im1 + a4 * z2im1 + 4 * h * z1im1 +
        4 * z2im1;
    static constexpr auto a8 = a2 * h;

    const auto z1i =
        a6 *
        (a5 * (-a0 * z1im1 - a8 * z2im1 + h * xi + h * xim1 + 2 * z1im1) -
         a7 * a8) /
        (a0 + 2 * mp::one);
    const auto z2i = a6 * a7;
    const auto yi = (z1i - z1im1) / h;

    prev_unfiltered = xi;
    prev_state = {z1i, z2i};

    return return_type{yi, prev_state};
  }
};

}  // namespace filter
