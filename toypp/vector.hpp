#ifndef TOYPP_VECTOR_HPP
#define TOYPP_VECTOR_HPP

#include <cstddef>
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

  template <typename U, std::size_t M>
  Vector(const Vector<U, M>& vec) { /* TODO implement */ }

  constexpr auto size() const noexcept { return N; }

  constexpr auto& at(std::size_t idx)
  {
    if (idx >= size()) // TODO throw error...
      return value_type{};

    return arr[idx];
  }

  constexpr const auto& at(std::size_t idx) const
  {
    if (idx >= size()) // TODO throw error...
      return value_type{};

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
};

}  // namespace tpp

#endif  // TOYPP_VECTOR_HPP
