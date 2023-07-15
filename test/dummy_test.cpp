#include <boost/ut.hpp>

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  test("true is true") = [] { expect(true); };
}
