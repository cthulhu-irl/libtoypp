#ifndef TOYPP_MATRIX_HPP
#define TOYPP_MATRIX_HPP

#include <cstddef>
#include <array>
#include <utility>
#include <type_traits>

#include "math.hpp"
#include "range.hpp"

namespace tpp {

template <typename T, std::size_t N, std::size_t M>
struct Matrix {
  static_assert(
    N > 0 && M > 0,
    "row or column size of matrix can't be zero."
  );

  using value_type = T;
  using array_type = value_type[N][M];

  constexpr static auto row_size = N;
  constexpr static auto col_size = M;

  array_type arr {};

  constexpr auto& get(std::size_t n, std::size_t m) noexcept
  {
    return arr[n][m];
  }

  constexpr const auto& get(std::size_t n, std::size_t m) const noexcept
  {
    return arr[n][m];
  }

  template <typename U>
  constexpr auto& set(std::size_t n, std::size_t m, U&& value)
  {
    arr[n][m] = std::forward<U>(value);
    return *this;
  }

  constexpr auto det() const noexcept
  {
    static_assert(
      row_size == col_size,
      "determinant of a non-square matrix is not defined."
    );

    if constexpr (row_size == 1 && col_size == 1)
      return arr[0][0];

    else if constexpr (row_size == 2 && col_size == 2)
      return arr[0][0] * arr[1][1] - arr[0][1] * arr[1][0];

    constexpr auto trcolidx = [](auto i, auto j, auto n)
    {
      return (i + j) % n;
    };

    constexpr auto invtrcolidx = [](auto i, auto j, auto n)
    {
      if (i > j) return n - (i - j);
      else       return j - i;
    };

    value_type tr_total{};
    value_type invtr_total{};

    for (auto j : Range<std::size_t>(0, col_size, 1)) {
      value_type tr{1};
      value_type invtr{1};

      for (auto i : Range<std::size_t>(0, row_size, 1))
      {
        tr    *= arr[i][   trcolidx(i, j, col_size)];
        invtr *= arr[i][invtrcolidx(i, j, col_size)];
      }

      tr_total    += tr;
      invtr_total += invtr;
    }

    return tr_total - invtr_total;
  }

  template <typename U>
  constexpr auto sum(Matrix<U, row_size, col_size> other) const noexcept
  {
    for (auto i : Range<std::size_t>(0, row_size, 1))
      for (auto j : Range<std::size_t>(0, col_size, 1))
        other.arr[i][j] += std::move(arr[i][j]);

    return other;
  }

  template <typename U>
  constexpr auto sub(Matrix<U, row_size, col_size> other) const noexcept
  {
    for (auto i : Range<std::size_t>(0, row_size, 1))
      for (auto j : Range<std::size_t>(0, col_size, 1))
        other.arr[i][j] -= std::move(arr[i][j]);

    return other;
  }

  template <typename U, std::size_t P>
  constexpr auto mul(Matrix<U, M, P> other) const noexcept
  {
    using item_type = decltype(std::declval<U>() * std::declval<T>());

    constexpr auto rows = N;
    constexpr auto mids = M;
    constexpr auto cols = P;

    Matrix<item_type, rows, cols> ret{};

    for (auto i : Range<std::size_t>(0, rows, 1))
      for (auto j : Range<std::size_t>(0, cols, 1))
        for (auto k : Range<std::size_t>(0, mids, 1))
          ret[i][k] += get(i, k) * other.get(k, j);

    return ret;
  }

  template <typename U,
            std::size_t N1, std::size_t M1,
            std::size_t N2, std::size_t M2>
  constexpr auto subset() const noexcept
  {
    static_assert(N2 > N1, "N2 cannot be less than or equal to N1.");
    static_assert(M2 > M1, "M2 cannot be less than or equal to M1.");

    static_assert(
        (N2 - N1) <= N, "can't get a subset matrix with more rows.");
    static_assert(
        (M2 - M1) <= M, "can't get a subset matrix with more cols.");

    constexpr auto rows = N2 - N1;
    constexpr auto cols = M2 - M1;

    Matrix<U, rows, cols> ret;

    for (auto i : Range<std::size_t>(N1, N2, 1))
      for (auto j : Range<std::size_t>(M1, M2, 1))
        ret.get(i, j) = std::move(arr[i][j]);

    return ret;
  }

  template <typename U, std::size_t W, std::size_t H>
  constexpr auto subset() const noexcept
  {
    return subset<U, 0, 0, W, H>();
  }

  template <std::size_t N1, std::size_t M1,
            std::size_t N2, std::size_t M2,
            typename U, std::size_t X, std::size_t Y>
  constexpr auto overlay(Matrix<U, X, Y> other) const noexcept
  {
    static_assert(N2 > N1, "N2 cannot be less than or equal to N1.");
    static_assert(M2 > M1, "M2 cannot be less than or equal to M1.");

    static_assert(X >= N2, "N2 must be within 0 to X size.");
    static_assert(Y >= M2, "M2 must be within 0 to Y size.");

    static_assert(
        (N2 - N1) <= N, "can't overlay a matrix with more rows.");
    static_assert(
        (M2 - M1) <= M, "can't overlay a matrix with more cols.");

    constexpr auto rows = N2 - N1;
    constexpr auto cols = M2 - M1;

    Matrix<U, N, M> ret = *this;

    for (auto i : Range<std::size_t>(0, rows, 1))
      for (auto j : Range<std::size_t>(0, cols, 1))
        ret.get(i, j) = std::move(other.get(N1 + i, M1 + j));

    return ret;
  }
};

}  // namespace tpp

#endif  // TOYPP_MATRIX_HPP
