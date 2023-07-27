#include "complementary_filter/complementary_filter.hpp"

#include <boost/ut.hpp>

#include <mp-units/systems/si/si.h>

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  test("complementary filter test") = [] {
    using namespace mp_units::si::unit_symbols;

    // NOLINTBEGIN(readability-magic-numbers)
    {
      auto lowpass = complementary_filter::lowpass<20 * Hz, 5 * ms>{};
      [[maybe_unused]] const auto [y0, yd0] = lowpass(1.0 * rad);
    }

    {
      auto highpass = complementary_filter::highpass<20 * Hz, 5 * ms>{};
      [[maybe_unused]] const auto [y0, z0] = highpass(1.0 * rad);
    }
    // NOLINTEND(readability-magic-numbers)
  };
}
