#ifndef TOYPP_UNIQUEPTR_HPP_
#define TOYPP_UNIQUEPTR_HPP_

#include <cstddef>
#include <cstdio>
#include <memory>
#include <type_traits>
#include <utility>

namespace tpp {

namespace detail {

template <typename Deleter>
constexpr static bool is_defaultable_deleter =
  std::is_nothrow_default_constructible<Deleter>::value
  && !std::is_pointer<Deleter>::value;

}  // namespace detail

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
 public:
  using value_type = std::remove_reference_t<T>;

  using pointer = std::add_pointer_t<value_type>;
  using element_type = value_type;
  using deleter_type = Deleter;

 private:
  pointer ptr_ = nullptr;
  deleter_type deleter_{};

 public:
  template <typename DeleterA = Deleter,
            std::enable_if_t<
              detail::is_defaultable_deleter<DeleterA>, bool> = true>
  constexpr UniquePtr() noexcept {}

  template <typename DeleterA = Deleter,
            std::enable_if_t<
              detail::is_defaultable_deleter<DeleterA>, bool> = true>
  constexpr UniquePtr(std::nullptr_t) noexcept {}

  template <typename DeleterA = Deleter,
            std::enable_if_t<
              detail::is_defaultable_deleter<DeleterA>, bool> = true>
  constexpr explicit UniquePtr(pointer ptr) noexcept : ptr_(ptr) {}

  template <typename DeleterA = Deleter,
            std::enable_if_t<
              !(std::is_const<DeleterA>::value
                  && std::is_lvalue_reference<DeleterA>::value)
              && std::is_constructible<Deleter, Deleter>::value,
              bool> = true>
  constexpr UniquePtr(pointer ptr, const Deleter& deleter) noexcept
    : ptr_(ptr)
    , deleter_(deleter)
  {}

  template <typename DeleterA = Deleter,
            std::enable_if_t<
              !std::is_reference<DeleterA>::value
              && std::is_nothrow_move_constructible<Deleter>::value,
              bool> = true>
  constexpr UniquePtr(pointer ptr, Deleter& deleter) noexcept
    : ptr_(ptr)
    , deleter_(deleter)
  {}

  template <typename DeleterA = Deleter,
            std::enable_if_t<
              std::is_reference<DeleterA>::value
              && std::is_constructible<Deleter, Deleter>::value,
              bool> = true>
  constexpr UniquePtr(
      pointer ptr,
      std::remove_reference_t<Deleter>&& deleter) noexcept = delete;

  template <typename DeleterA = Deleter,
            std::enable_if_t<
              !std::is_reference<DeleterA>::value
              && std::is_constructible<Deleter, Deleter&&>::value,
              bool> = true>
  constexpr UniquePtr(pointer ptr, Deleter&& deleter) noexcept
    : ptr_(ptr)
    , deleter_(std::forward<Deleter>(deleter))
  {}

  template <typename U, typename E,
            std::enable_if_t<
              std::is_convertible<
                typename UniquePtr<U, E>::pointer, pointer>::value
              && !std::is_array<U>::value
              && std::is_nothrow_constructible<Deleter, E>::value,
              bool> = true>
  constexpr UniquePtr& operator=(UniquePtr<U, E>&& other) noexcept
  {
    ptr_ = other.release();
    deleter_ = std::exchange(deleter_, std::forward<E>(other.deleter_));
  }

  template <typename DeleterA = Deleter,
            std::enable_if_t<
              std::is_nothrow_move_constructible<DeleterA>::value,
              bool> = true>
  constexpr UniquePtr& operator=(UniquePtr&& other) noexcept
  {
    std::swap(ptr_, other.ptr_);
    std::swap(deleter_, other.deleter_);
  }

  constexpr UniquePtr& operator=(std::nullptr_t) noexcept
  {
    reset();
    return *this;
  }

  template <typename U, typename E,
            std::enable_if_t<
              !std::is_array<U>::value
              && std::is_convertible<
                    typename UniquePtr<U, E>::pointer, pointer>::value
              && std::is_constructible<Deleter, E>::value,
              bool> = true>
  constexpr UniquePtr(UniquePtr<U, E>&& other) noexcept
    : ptr_(std::exchange(other.ptr_, nullptr))
    , deleter_(std::exchange(other.deleter_, deleter_type{}))
  {}

  template <typename U, typename E,
            std::enable_if_t<
              !std::is_array<U>::value
              && std::is_convertible<
                    typename UniquePtr<U, E>::pointer, pointer>::value
              && std::is_constructible<Deleter, E>::value,
              bool> = true>
  constexpr UniquePtr(const UniquePtr<U, E>&) noexcept = delete;
  constexpr UniquePtr(const UniquePtr&) noexcept = delete;
  constexpr UniquePtr(UniquePtr&&) noexcept = default;

  template <typename U, typename E,
            std::enable_if_t<
              !std::is_array<U>::value
              && std::is_convertible<
                    typename UniquePtr<U, E>::pointer, pointer>::value
              && std::is_constructible<Deleter, E>::value,
              bool> = true>
  constexpr UniquePtr& operator=(const UniquePtr<U, E>&) noexcept = delete;
  constexpr UniquePtr& operator=(const UniquePtr&) noexcept = delete;
  constexpr UniquePtr& operator=(UniquePtr&&) noexcept = default;

  ~UniquePtr() noexcept { deleter_(ptr_); }

  constexpr pointer release() noexcept
  {
    return std::exchange(ptr_, nullptr);
  }

  constexpr void reset(pointer other = nullptr) noexcept
  {
    auto old = std::exchange(ptr_, other);
    if (old) deleter_(old);
  }

  constexpr void swap(pointer ptr) noexcept { std::swap(ptr_, ptr); }
  constexpr void swap(UniquePtr& other) noexcept
  {
    std::swap(ptr_, other.ptr_);
    std::swap(deleter_, other.deleter_);
  }

  constexpr auto get() noexcept { return ptr_; }
  constexpr auto get() const noexcept { return ptr_; }

  constexpr auto& get_deleter() & noexcept { return deleter_; }
  constexpr const auto& get_deleter() const& noexcept { return deleter_; }

  constexpr auto&& get_deleter() && noexcept { return std::move(deleter_); }
  constexpr const auto&& get_deleter() const&& noexcept
  {
    return std::move(deleter_);
  }

  constexpr auto& operator[](std::size_t index) noexcept
  {
    return ptr_[index];
  }

  constexpr const auto& operator[](std::size_t index) const noexcept
  {
    return ptr_[index];
  }

  constexpr auto&  operator*() &  noexcept { return *ptr_; }
  constexpr const auto& operator*() const& noexcept { return *ptr_;  }

  constexpr auto&& operator*() && noexcept { return std::move(*ptr_); }
  constexpr const auto&& operator*() const&& noexcept
  {
    return std::move(*ptr_);
  }

  constexpr pointer operator->() noexcept { return ptr_; }
  constexpr const pointer operator->() const noexcept { return ptr_; }

  constexpr bool operator==(const UniquePtr& other) const noexcept
  {
    return ptr_ == other.ptr_;
  }

  constexpr bool operator!=(const UniquePtr& other) const noexcept
  {
    return ptr_ != other.ptr_;
  }

  constexpr bool operator==(std::nullptr_t) const noexcept
  {
    return !ptr_;
  }

  constexpr bool operator!=(std::nullptr_t) const noexcept
  {
    return !!ptr_;
  }

  constexpr operator bool() const noexcept
  {
    return static_cast<bool>(ptr_);
  }
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
  // TODO implement this...
};

template <typename T,
          std::enable_if_t<std::is_array<T>::value
                           && std::extent<T>::value == 0, bool> = true>
UniquePtr<T> make_unique(const std::size_t size)
{
  return UniquePtr<T>(new std::remove_extent_t<T>[size]);
}

template <typename T,
          typename ...Args,
          std::enable_if_t<!std::is_array<T>::value, bool> = true>
UniquePtr<T> make_unique(Args&&... args)
{
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T,
          typename ...Args,
          std::enable_if_t<std::extent<T>::value != 0, bool> = true>
UniquePtr<T> make_unique(Args...) = delete;

}  // namespace tpp

#endif  // TOYPP_UNIQUEPTR_HPP_
