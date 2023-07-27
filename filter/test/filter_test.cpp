#include "filter/filter.hpp"

#include <boost/ut.hpp>

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

// NOLINTBEGIN(readability-magic-numbers)
constexpr auto cutoff_freq = constant{20};       // Hz;
constexpr auto sample_period = constant{0.005};  // ms;
// NOLINTEND(readability-magic-numbers)

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  test("high pass filter invocable") = [] {
    auto highpass = filter::highpass<cutoff_freq, sample_period>{};
    [[maybe_unused]] const auto [y0, z0] = highpass(1.0);
    [[maybe_unused]] const auto _ = y0;
  };

  test("low pass filter invocable") = [] {
    auto lowpass = filter::lowpass<cutoff_freq, sample_period>{};
    [[maybe_unused]] const auto [y0, yd0] = lowpass(1.0);
    [[maybe_unused]] const auto _ = y0;
  };
}
