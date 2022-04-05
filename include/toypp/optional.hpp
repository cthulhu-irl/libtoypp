#ifndef TOYPP_OPTIONAL_HPP_
#define TOYPP_OPTIONAL_HPP_

#include <type_traits>
#include <variant>
#include <utility>

namespace tpp {

namespace optional_detail {

template <typename ...Fs>
struct overloaded : Fs... { using Fs::operator()...; };

template <typename ...Fs>
overloaded(Fs&& ...) -> overloaded<Fs...>;

}  // namespace optional_detail

struct nullopt_t {};
constexpr static nullopt_t nullopt = {};


template <typename T>
class optional {
 public:
  using value_type = std::remove_reference_t<std::remove_cv_t<T>>;
  using pointer = std::add_pointer_t<value_type>;

 private:
  using variant_type = std::variant<T, nullopt_t>;

  variant_type value_;

 public:
  constexpr optional(nullopt_t) noexcept : value_(nullopt) {}

  constexpr optional(const T& value) noexcept(std::is_nothrow_move_constructible_v<T>)
    : value_(value)
  {}

  constexpr optional(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
    : value_(std::move(value))
  {}

  template <typename U, std::enable_if_t<std::is_constructible_v<T, U>, bool> = true>
  constexpr optional(U&& value) noexcept(std::is_nothrow_constructible_v<T, U>)
    : value_(std::forward<U>(value))
  {}

  template <typename U, std::enable_if_t<std::is_constructible_v<T, U>, bool> = true>
  constexpr optional(const optional<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>)
    : value_(other.value_)
  {}

  template <typename U, std::enable_if_t<std::is_constructible_v<T, U>, bool> = true>
  constexpr optional(optional<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>)
    : value_(std::move(other.value_))
  {}

  template <typename ...Args>
  constexpr explicit optional(std::in_place_t, Args&& ...args)
    : value_(std::in_place_type_t<T>{}, std::forward<Args>(args)...)
  {}

  template <typename U, typename ...Args>
  constexpr explicit optional(std::in_place_t, std::initializer_list<U> ilist, Args&& ...args)
    : value_(std::in_place_type_t<T>{}, std::move(ilist), std::forward<Args>(args)...)
  {}

  // --

  constexpr void swap(optional& other) noexcept(std::is_nothrow_swappable_v<variant_type>) {
    std::swap(value_, other.value_);
  }

  constexpr void reset() noexcept { value_ = nullopt; }

  template <typename ...Args>
  constexpr T& emplace(Args&& ...args) {
    value_.template emplace<T>(std::forward<Args>(args)...);
  }

  template <typename U, typename ...Args>
  constexpr T& emplace(std::initializer_list<U> ilist, Args&& ...args) {
    return value_.template emplace<T>(std::move(ilist), std::forward<Args>(args)...);
  }

  constexpr bool has_value() const noexcept {
    return std::visit(
      optional_detail::overloaded{
        [](nullopt_t) { return false; },
        [](const T&) { return true; }
      },
      value_
    );
  }

  constexpr       T&  value() &       noexcept { return std::get<T>(value_); }
  constexpr const T&  value() const&  noexcept { return std::get<T>(value_); }
  constexpr       T&& value() &&      noexcept { return std::get<T>(value_); }
  constexpr const T&& value() const&& noexcept { return std::get<T>(value_); }

  template <typename U>
  constexpr T value_or(U&& default_value) const& {
    return std::visit(
      optional_detail::overloaded{
        [&default_value](nullopt_t) { return T(std::forward<U>(default_value)); },
        [](const T& value) { return value; }
      },
      value_
    );
  }

  template <typename U>
  constexpr T value_or(U&& default_value) && {
    return std::visit(
      optional_detail::overloaded{
        [&default_value](nullopt_t) { return T(std::forward<U>(default_value)); },
        [](T& value) { return std::move(value); }
      },
      value_
    );
  }

  friend constexpr bool operator==(const optional& opt, std::nullptr_t) noexcept {
    return opt.has_value();
  }

  friend constexpr bool operator==(std::nullptr_t, const optional& opt) noexcept {
    return opt.has_value();
  }

  template <typename U>
  friend constexpr bool operator==(const optional& lhs, U&& rhs) {
    return lhs.has_value() && lhs.value_ == rhs;
  }

  template <typename U>
  friend constexpr bool operator==(U&& lhs, const optional& rhs) {
    return rhs.has_value() && lhs == rhs.value_;
  }

  constexpr       T&  operator*() &       noexcept { return std::get<T>(value_); }
  constexpr const T&  operator*() const&  noexcept { return std::get<T>(value_); }
  constexpr       T&& operator*() &&      noexcept { return std::get<T>(value_); }
  constexpr const T&& operator*() const&& noexcept { return std::get<T>(value_); }

  constexpr pointer operator->() noexcept { return std::addressof(std::get<T>()); }
  constexpr const pointer operator->() const noexcept { return std::addressof(std::get<T>()); }

  constexpr explicit operator bool() const noexcept { return has_value(); }
};

template <typename T> class optional<T&> {
  static_assert(sizeof(T) > 0, "optional<T&> is not implemented.");
};

template <typename T> class optional<T&&> {
  static_assert(sizeof(T) > 0, "optional<T&&> is not implemented.");
};

}  // namespace tpp

#endif  // TOYPP_OPTIONAL_HPP_
