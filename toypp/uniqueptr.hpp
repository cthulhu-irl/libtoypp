#ifndef TOYPP_UNIQUEPTR_HPP_
#define TOYPP_UNIQUEPTR_HPP_

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace tpp {

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr final {
 public:
  using value_type = std::remove_reference_t<std::decay_t<T>>;
  using pointer_type = value_type*;
  using const_pointer_type = const value_type*;
  using reference_type = value_type&;
  using const_reference_type = const value_type&;

  using deleter_type = Deleter;

 private:
  pointer_type ptr_ = nullptr;
  deleter_type deleter_{};

 public:
  constexpr UniquePtr() {}
  constexpr UniquePtr(std::nullptr_t) {}

  constexpr explicit UniquePtr(pointer_type ptr) noexcept : ptr_(ptr) {}

  constexpr UniquePtr(pointer_type ptr, deleter_type deleter) noexcept
    : ptr_(ptr), deleter_(deleter)
  {}

  constexpr UniquePtr(const UniquePtr&) = delete;
  constexpr UniquePtr& operator=(const UniquePtr&) = delete;

  constexpr UniquePtr(UniquePtr&& other) noexcept
    : ptr_(std::exchange(other.ptr_, nullptr))
    , deleter_(std::exchange(other.deleter_, deleter_type()))
  {}

  constexpr UniquePtr& operator=(UniquePtr&&) noexcept = default;
  constexpr UniquePtr& operator=(std::nullptr_t) noexcept
  {
    ptr_ = nullptr;
  }

  ~UniquePtr() { deleter_(ptr_); }

  constexpr pointer_type release() noexcept
  {
    return std::exchange(ptr_, nullptr);
  }

  constexpr void reset() noexcept { ptr_ = nullptr; }

  constexpr void swap(pointer_type ptr) noexcept { std::swap(ptr_, ptr); }
  constexpr void swap(UniquePtr& other) noexcept
  {
    std::swap(ptr_, other.ptr_);
    std::swap(deleter_, other.deleter_);
  }

  constexpr pointer_type get() noexcept { return ptr_; }
  constexpr const_pointer_type get() const noexcept { return ptr_; }

  constexpr deleter_type& get_deleter() noexcept { return deleter_; }
  constexpr const deleter_type& get_deleter() const noexcept
  {
    return deleter_;
  }

  constexpr auto& operator[](std::size_t index) noexcept
  {
    static_assert(std::is_array<T>::value, "T is not an array.");
    return ptr_[index];
  }

  constexpr const auto& operator[](std::size_t index) const noexcept
  {
    static_assert(std::is_array_v<T>, "T is not an array.");
    return ptr_[index];
  }

  constexpr reference_type operator*() noexcept { return *ptr_; }
  constexpr const_reference_type operator*() const noexcept
  {
    return *ptr_;
  }

  constexpr pointer_type operator->() noexcept { return ptr_; }
  constexpr const_pointer_type operator->() const noexcept { return ptr_; }

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

  constexpr operator bool() const noexcept { return bool(ptr_); }
};

template <typename T, typename ...Args>
std::enable_if_t<!std::is_array_v<T>, UniquePtr<T>>
make_unique(Args&&... args)
{
  return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
std::enable_if_t<std::is_array_v<T>, UniquePtr<T>>
make_unique(std::size_t size)
{
  return UniquePtr<T>(new std::remove_extent_t<T>[size]);
}

}  // namespace tpp

#endif  // TOYPP_UNIQUEPTR_HPP_
