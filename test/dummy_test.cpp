#include <boost/ut.hpp>

#include <array>
#include <cassert>
#include <cstddef>
#include <experimental/linalg>
#include <experimental/mdspan>
#include <fmt/core.h>

namespace stx = std::experimental;

template <class T, std::size_t R, std::size_t C>
struct matrix
{
  static_assert(R != std::dynamic_extent);
  static_assert(C != std::dynamic_extent);

  using extents_type = stx::extents<std::size_t, R, C>;
  using mdspan_type = stx::mdspan<T, extents_type>;
  using const_mdspan_type = stx::mdspan<const T, extents_type>;

  std::array<T, R * C> data{};

  matrix() = default;

  matrix(std::initializer_list<std::initializer_list<T>> init)
  {
    assert(R == init.size());

    auto i = std::size_t{};
    for (const auto& row : init) {
      assert(C == row.size());

      auto j = std::size_t{};
      for (const auto& elem : row) {
        span()[i, j] = elem;

        ++j;
      }

      ++i;
    }
  }

  [[nodiscard]]
  constexpr auto span() -> mdspan_type
  {
    return mdspan_type{data.data()};
  }
  [[nodiscard]]
  constexpr auto span() const -> const_mdspan_type
  {
    return const_mdspan_type{data.data()};
  }

  [[nodiscard]]
  constexpr auto
  operator[](std::size_t i, std::size_t j) -> mdspan_type::reference
  {
    return span()[i, j];
  }
  [[nodiscard]]
  constexpr auto
  operator[](std::size_t i, std::size_t j) const -> const_mdspan_type::reference
  {
    return span()[i, j];
  }
};

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  // NOLINTBEGIN(readability-magic-numbers)
  test("true is true") = [] {
    using Mat = matrix<int, 2, 2>;

    auto A = Mat{{1, 2}, {3, 4}};
    auto B = Mat{{5, 6}, {7, 8}};
    auto C = Mat{};

    stx::linalg::matrix_product(A.span(), B.span(), C.span());

    const auto s = fmt::format("The answer is {}.", C[0, 0]);

    expect("The answer is 19." == s);
  };
  // NOLINTEND(readability-magic-numbers)
}
