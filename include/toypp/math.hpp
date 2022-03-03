#ifndef TOYPP_MATH_HPP
#define TOYPP_MATH_HPP

#include <cstddef>
#include <utility>

namespace tpp {

template <typename T>
constexpr auto clamp(const T& min, const T& max, const T& value) noexcept
{
  if (value >= max) return max;
  if (value <= min) return min;

  return value;
}

template <typename T, typename ...Ts>
constexpr auto sum(T&& x, Ts&&... xs) noexcept
{
  return (std::forward<T>(x) + ... + std::forward<Ts>(xs));
}

template <typename T, typename ...Ts>
constexpr auto prod(T&& x, Ts&&... xs) noexcept
{
  return (std::forward<T>(x) * ... * std::forward<Ts>(xs));
}

template <typename T, typename U>
constexpr auto lerp(const T& a, const U& b, double t) noexcept
{
  return a + (a - b) * t;
}

template <typename T, typename U>
constexpr double invlerp(const T& a, const T& b, const T& v) noexcept
{
  const auto diff = a - b;

  if (diff == 0) return 0;

  return clamp(0, 1, (v - a) / diff);
}

}  // namespace tpp

#endif  // TOYPP_MATH_HPP
