#ifndef TOYPP_VECTOR_HPP
#define TOYPP_VECTOR_HPP

#include <cstddef>
#include <stdexcept>
#include <array>
#include <utility>
#include <type_traits>

#include "math.hpp"
#include "range.hpp"

namespace tpp {

template <typename T, std::size_t N>
struct Vector {
  using value_type = T;

  value_type arr[N] {};

  constexpr auto size() const noexcept { return N; }

  template <std::size_t I>
  constexpr auto& at() noexcept {
    static_assert(I < N, "out-of-bounds access.");
    return arr[I];
  }

  template <std::size_t I>
  constexpr const auto& at() const noexcept {
    static_assert(I < N, "out-of-bounds access.");
    return arr[I];
  }

  constexpr auto& at(std::size_t idx)
  {
    if (idx >= size())
      throw std::out_of_range{"out-of-bounds access."};

    return arr[idx];
  }

  constexpr const auto& at(std::size_t idx) const
  {
    if (idx >= size())
      throw std::out_of_range{"out-of-bounds access."};

    return arr[idx];
  }

  constexpr auto sum() const noexcept
  {
    auto ret = at(0);

    for (auto i : Range<std::size_t>(1, size(), 1))
      ret += at(i);

    return ret;
  }

  constexpr auto cumsum() const noexcept
  {
    Vector<T, N> ret;

    ret.at(0) = at(0);

    for (auto i : Range<std::size_t>(1, size(), 1))
      ret.at(i) = ret.at(i-1) + at(i);

    return ret;
  }

  constexpr auto neg() const noexcept
  {
    using item_type = decltype(-std::declval<T>());

    Vector<item_type, N> ret;

    for (auto i : Range<std::size_t>(0, size(), 1))
      ret.at(i) = -at(i);

    return ret;
  }

  template <typename U>
  constexpr auto add(Vector<U, N> other) const noexcept
  {
    using item_type = decltype(std::declval<T>() + std::declval<U>());

    Vector<item_type, N> ret;

    for (auto i : Range<std::size_t>(0, size(), 1))
      ret.at(i) = at(i) + std::move(other.at(i));

    return ret;
  }

  template <typename U>
  constexpr auto sub(Vector<U, N> other) const noexcept
  {
    using item_type = decltype(std::declval<T>() + std::declval<U>());

    Vector<item_type, N> ret;

    for (auto i : Range<std::size_t>(0, size(), 1))
      ret.at(i) = at(i) - std::move(other.at(i));

    return ret;
  }

  template <typename U>
  constexpr auto mul(Vector<U, N> other) const noexcept
  {
    using item_type = decltype(std::declval<T>() * std::declval<U>());

    Vector<item_type, N> ret;

    for (auto i : Range<std::size_t>(0, size(), 1))
      ret.at(i) = at(i) * std::move(other.at(i));

    return ret;
  }

  template <typename U>
  constexpr auto mul(U other) const noexcept
  {
    using item_type = decltype(std::declval<T>() * std::declval<U>());

    Vector<item_type, N> ret;

    for (auto i : Range<std::size_t>(0, size(), 1))
      ret.at(i) = at(i) * other;

    return ret;
  }

  template <typename U>
  constexpr auto div(Vector<U, N> other) const noexcept
  {
    using item_type = decltype(std::declval<T>() * std::declval<U>());

    Vector<item_type, N> ret;

    for (auto i : Range<std::size_t>(0, size(), 1))
      ret.at(i) = at(i) / std::move(other.at(i));

    return ret;
  }

  constexpr auto operator-() const { return neg(); }

  template <typename U>
  constexpr auto operator+(U other) const { return add(std::move(other)); }

  template <typename U>
  constexpr auto operator-(U other) const { return sub(std::move(other)); }

  template <typename U>
  constexpr auto operator*(U other) const { return mul(std::move(other)); }

  template <typename U>
  constexpr auto operator/(U other) const { return div(std::move(other)); }
};

template <typename U, typename T, std::size_t N>
constexpr auto operator*(U lhs, Vector<T, N> rhs)
{
  return rhs * lhs;
}

}  // namespace tpp

#endif  // TOYPP_VECTOR_HPP
