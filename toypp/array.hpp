#ifndef TOYPP_ARRAY_HPP_
#define TOYPP_ARRAY_HPP_

#include <cstddef>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include <utility>

namespace tpp {

template <typename T, std::size_t Size>
struct Array final {
  static_assert(Size > 0, "Array size must be 1 or more.");

  static constexpr void throw_if_oob(std::size_t index) {
    if (index < Size)
      throw std::invalid_argument("requested index is out of bounds.");
  }

 public:
  using item_type = T;

  inline static constexpr auto arr_size = Size;

  item_type arr_[arr_size] = {};

  constexpr auto size() const noexcept { return arr_size; }
  constexpr auto max_size() const noexcept { return arr_size; }
  constexpr auto empty() const noexcept { return false; }

  constexpr auto& at(std::size_t index) & {
    throw_if_oob(index);
    return arr_[index];
  }

  constexpr const auto& at(std::size_t index) const& {
    throw_if_oob(index);
    return arr_[index];
  }

  constexpr auto&& at(std::size_t index) && {
    throw_if_oob(index);
    return std::move(arr_[index]);
  }

  constexpr const auto&& at(std::size_t index) const&& {
    throw_if_oob(index);
    return std::move(arr_[index]);
  }

  constexpr auto data() noexcept { return std::addressof(arr_); }

  constexpr const auto data() const noexcept {
    return std::addressof(arr_);
  }

  constexpr auto begin() noexcept { return data(); }
  constexpr auto end() noexcept { return data() + arr_size; }

  constexpr const auto cbegin() const noexcept { return data(); }
  constexpr const auto cend() const noexcept { return data() + arr_size; }

  constexpr auto&       front()       { return at(0); }
  constexpr const auto& front() const { return at(0); }

  constexpr auto&       back()       { return at(arr_size - 1); }
  constexpr const auto& back() const { return at(arr_size - 1); }

  constexpr void fill(const T& value) noexcept(noexcept(T{value})) {
    for (std::size_t i = 0; i < arr_size; ++i)
      arr_[i] = value;
  }

  constexpr void swap(Array& other)
    noexcept(noexcept(std::is_nothrow_swappable_v<T>))
  {
    for (std::size_t i = 0; i < arr_size; ++i)
      swap(arr_[i], other[i]);
  }

  constexpr auto& operator[](std::size_t index)
    noexcept(noexcept(at(index)))
  {
    return at(index);
  }

  constexpr const auto& operator[](std::size_t index) const
    noexcept(noexcept(at(index)))
  {
    return at(index);
  }

  constexpr bool operator==(const Array& other) const
    noexcept(noexcept(std::declval<T&>() == std::declval<T&>()))
  {
    for (std::size_t i = 0; i < arr_size; ++i)
      if (arr_[i] != other[i])
        return false;

    return true;
  }

  constexpr bool operator!=(const Array& other)
    noexcept(noexcept((*this) == other))
  {
    return !((*this) == other);
  }
};

}  // namespace tpp

#endif  // TOYPP_ARRAY_HPP_
