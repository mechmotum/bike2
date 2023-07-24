#include <boost/ut.hpp>

#include <fmt/core.h>
#include <mp-units/format.h>
#include <mp-units/ostream.h>
#include <mp-units/systems/international/international.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si/si.h>
#include <sstream>

using namespace mp_units;

constexpr QuantityOf<isq::speed> auto
avg_speed(QuantityOf<isq::length> auto d, QuantityOf<isq::time> auto t)
{
  return d / t;
}

constexpr auto to_string(const auto& value)
{
  auto ss = std::stringstream{};
  ss << value;
  return ss.str();
}

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  test("units example") = [] {
    using namespace mp_units::si::unit_symbols;
    using namespace mp_units::international::unit_symbols;

    constexpr auto v1 = 110 * (km / h);
    constexpr auto v2 = 70 * mph;
    constexpr auto v3 = avg_speed(220. * isq::distance[km], 2 * h);
    constexpr auto v4 = avg_speed(isq::distance(140. * mi), 2 * h);
    constexpr auto v5 = v3[m / s];
    constexpr auto v6 = value_cast<m / s>(v4);
    constexpr auto v7 = value_cast<int>(v6);

    expect("110 km/h" == to_string(v1));
    expect("70 mi/h" == to_string(v2));
    expect("110 km/h" == fmt::format("{}", v3));
    expect("***70 mi/h****" == fmt::format("{:*^14}", v4));
    expect("30.5556 in m/s" == fmt::format("{:%Q in %q}", v5));
    expect("31.2928 in m/s" == fmt::format("{0:%Q} in {0:%q}", v6));
    expect("31" == fmt::format("{:%Q}", v7));
  };
}
