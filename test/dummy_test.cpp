#include <boost/ut.hpp>

#include <fmt/core.h>
#include <armadillo>

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  test("true is true") = [] {
    const auto s = fmt::format("The answer is {}.", 42);

    expect("The answer is 42." == s);
  };
}
