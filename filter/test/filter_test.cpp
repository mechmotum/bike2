#include "filter/filter.hpp"

#include <boost/ut.hpp>

#include <mp-units/systems/si/si.h>

using namespace mp_units::si::unit_symbols;

// NOLINTBEGIN(readability-magic-numbers)
constexpr auto cutoff_freq = 20 * Hz;
constexpr auto sample_period = 5 * ms;
// NOLINTEND(readability-magic-numbers)

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  test("high pass filter invocable") = [] {
    auto highpass = filter::highpass<cutoff_freq, sample_period>{};
    [[maybe_unused]] const auto [y0, z0] = highpass(1.0 * rad);
    [[maybe_unused]] const auto _ = y0.number();
  };

  test("low pass filter invocable") = [] {
    auto lowpass = filter::lowpass<cutoff_freq, sample_period>{};
    [[maybe_unused]] const auto [y0, yd0] = lowpass(1.0 * rad);
    [[maybe_unused]] const auto _ = y0.number();
  };
}
