#ifndef TOYPP_SPAN_HPP_
#define TOYPP_SPAN_HPP_

namespace tpp {

template <typename T>
class Span {
 public:
  using value_type = std::remove_reference_t<T>;
  using pointer = std::add_pointer_t<value_type>;
  using const_pointer = const pointer;
  using reference = value_type&;
  using const_reference = const value_type&;

 private:
  pointer ptr_ = nullptr;
  std::size_t size_ = 0;

 public:
  constexpr Span(pointer ptr, std::size_t size) noexcept
    : ptr_(ptr)
    , size_(size)
  {}

  [[nodiscard]] constexpr auto data() const noexcept -> pointer { return ptr_; }

  [[nodiscard]] constexpr auto size() const noexcept -> std::size_t { return size_; }
  [[nodiscard]] constexpr auto size_bytes() const noexcept -> std::size_t { return size_ * sizeof(value_type); }

  [[nodiscard]] constexpr auto empty() const noexcept -> bool { return size_ == 0; }

  [[nodiscard]] constexpr auto front() const noexcept -> reference { return ptr_[0]; }
  [[nodiscard]] constexpr auto back() const noexcept -> reference { return ptr_[size_-1]; }

  [[nodiscard]] constexpr auto begin() noexcept -> pointer { return ptr_; }
  [[nodiscard]] constexpr auto begin() const noexcept -> const_pointer { return ptr_; }
  [[nodiscard]] constexpr auto end() noexcept -> pointer { return ptr_ + size_; }
  [[nodiscard]] constexpr auto end() const noexcept -> const_pointer { return ptr_ + size_; }

  [[nodiscard]] constexpr auto subspan(std::size_t offset) const noexcept -> Span
  {
    return Span(ptr_ + offset, size_ - offset);
  }

  [[nodiscard]] constexpr auto subspan(std::size_t offset, std::size_t count) const noexcept -> Span
  {
    return Span(ptr_ + offset, count);
  }

  [[nodiscard]] constexpr auto at(std::size_t index) const noexcept -> reference { return ptr_[index]; }
  [[nodiscard]] constexpr auto operator[](std::size_t index) const noexcept -> reference { return ptr_[index]; }

  [[nodiscard]] constexpr operator Span<const T>() const noexcept { return Span<const T>(ptr_, size_); }
};

}  // namespace tpp

#endif  // TOYPP_SPAN_HPP_
