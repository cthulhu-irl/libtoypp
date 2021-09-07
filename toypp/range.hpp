#ifndef TOYPP_RANGE_HPP_
#define TOYPP_RANGE_HPP_

#include <type_traits>
#include <algorithm>
#include <utility>

namespace tpp {

namespace detail {

template <typename T>
class RangeIterator final {
 public:
  using iterator = RangeIterator<T>;
  using value_type = std::remove_reference_t<T>;
  using pointer_type = std::add_pointer_t<value_type>;
  using reference_type = value_type&;

 private:
  value_type iter_;
  value_type start_;
  value_type end_;
  value_type step_;

  constexpr RangeIterator(T init)
    : iter_(std::move(init)), start_{}, end_{}, step_{}
  {}

 public:
  constexpr RangeIterator(T init, T end, T step)
    : iter_(init)
    , start_(std::move(init))
    , end_(std::move(end))
    , step_(std::move(step))
  {
    if (step < 0)
      std::swap(start_, end_);

    if (start_ >= end_)
      iter_ = end_;

    else if (std::clamp(iter_ + step_, start_, end_) == start_)
      iter_ = end_;
  }

  constexpr pointer_type operator->()
  {
    return std::addressof(iter_);
  }

  constexpr const pointer_type operator->() const
  {
    return std::addressof(iter_);
  }

  constexpr       value_type& operator*()       { return iter_; }
  constexpr const value_type& operator*() const { return iter_; }

  constexpr iterator operator++()
  {
    iter_ = std::clamp(iter_ + step_, start_, end_);
    return *this;
  }

  constexpr iterator operator++(int)
  {
    iterator tmp = *this;
    iter_ = std::clamp(iter_ + step_, start_, end_);
    return tmp;
  }

  constexpr iterator operator--()
  {
    iter_ = std::clamp(iter_ - step_, start_, end_);
    return *this;
  }

  constexpr iterator operator--(int)
  {
    iterator tmp = *this;
    iter_ = std::clamp(iter_ - step_, start_, end_);
    return tmp;
  }

  constexpr iterator operator+(const iterator& other) const
  {
    return std::clamp(iter_ + other * step_, start_, end_);
  }

  constexpr iterator operator-(const iterator& other) const
  {
    return std::clamp(iter_ - other * step_, start_, end_);
  }

  constexpr iterator operator+=(const iterator& other)
  {
    iter_ = std::clamp(iter_ + other * step_, start_, end_);
    return *this;
  }

  constexpr iterator operator-=(const iterator& other)
  {
    iter_ = std::clamp(iter_ - other * step_, start_, end_);
    return *this;
  }

  constexpr bool operator==(const iterator& other) const
  {
    return iter_ == other.iter_;
  }

  constexpr bool operator!=(const iterator& other) const
  {
    return iter_ != other.iter_;
  }

  constexpr bool operator<=(const iterator& other) const
  {
    return iter_ <= other.iter_;
  }

  constexpr bool operator>=(const iterator& other) const
  {
    return iter_ >= other.iter_;
  }

  constexpr bool operator<(const iterator& other) const
  {
    return iter_ < other.iter_;
  }

  constexpr bool operator>(const iterator& other) const
  {
    return iter_ > other.iter_;
  }
};

}  // namespace detail

template <typename T = int>
class Range {
 public:
  using iterator = detail::RangeIterator<T>;
  using const_iterator = const detail::RangeIterator<T>;

 private:
  const T start_{};
  const T end_{};
  const T step_{};

 public:
  constexpr Range() {}
  constexpr Range(T start, T end, T step = 1)
    : start_(std::move(start))
    , end_(std::move(end))
    , step_(std::move(step))
  {}

  constexpr T get_start() const { return start_; }
  constexpr T get_end()   const { return end_;   }
  constexpr T get_step()  const { return step_;  }

  constexpr iterator begin() const { return {start_, end_, step_}; }
  constexpr iterator end()   const { return {end_,   end_, step_}; }

  constexpr iterator cbegin() const { return {start_, end_, step_}; }
  constexpr iterator cend()   const { return {end_,   end_, step_}; }
};

}  // namespace tpp

#endif  // TOYPP_RANGE_HPP_
