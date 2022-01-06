#ifndef DYNAMIC_SQUARE_MATRIX_HPP_
#define DYNAMIC_SQUARE_MATRIX_HPP_

#include <cstddef>
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

template <typename T, typename Vector = std::vector<T>>
class DynamicSquareMatrix {
  Vector      vec_;
  std::size_t size_ = 0;

 public:
  using value_type = T;

  constexpr explicit DynamicSquareMatrix() {}
  constexpr explicit DynamicSquareMatrix(std::size_t n) { resize(n); }

  constexpr std::size_t size()     const noexcept { return size_;     }

  constexpr std::size_t width()  const noexcept { return size_; }
  constexpr std::size_t height() const noexcept { return size_; }

  constexpr void resize(std::size_t n, T value = T())
  {
    vec_.resize(n * n, std::move(value));
    size_ = n;
  }

  constexpr T& at(std::size_t n, std::size_t m)
  {
    // why the fuck there is no std::optional<T&>?
    // if (size <= n || size <= m) return std::nullopt;
    return vec_[index(n, m)];
  }

  constexpr const T& at(std::size_t n, std::size_t m) const
  {
    // why the fuck there is no std::optional<const T&>?
    // if (size <= n || size <= m) return std::nullopt;
    return vec_[index(n, m)];
  }

  // the iteration order won't be same as other matrices...
  // idx != (i * width + j)

  constexpr auto begin() { return std::begin(vec_); }
  constexpr auto end()   { return std::end(vec_);   }

  constexpr const auto cbegin() const { return std::cbegin(vec_); }
  constexpr const auto cend()   const { return std::cend(vec_);   }

  constexpr DynamicSquareMatrix subset(std::size_t n) const
  {
    DynamicSquareMatrix ret(n);

    std::size_t idx = 0;
    for (const auto& value : *this)
      *(ret.begin() + idx) = value;

    return ret;
  }

  constexpr T& operator()(std::size_t n, std::size_t m)
  {
    return at(n, m);
  }

  constexpr const T& operator()(std::size_t n, std::size_t m) const
  {
    return at(n, m);
  }

 private:
  constexpr std::size_t index(std::size_t n, std::size_t m) const noexcept
  {
    // inp: (0,0), (0,1),(1,0),(1,1), (0,2),(1,2),(2,0),(2,1)(2,2), ...
    // res:   0      1     2     3      4     5     6     7    8

    const auto max = (n > m) ? n : m;
    return max*max + (max == n) * m + n;
  }

  constexpr auto reverse_index(std::size_t idx) const noexcept
    -> std::pair<std::size_t, std::size_t>
  {
    const std::size_t n = std::sqrt(idx);
    const std::size_t sq = n*n;
    const std::size_t rem = idx - sq;
    const bool h = (rem+1)/(n+1);
    return {h*n + (!h)*rem, (!h)*n + h*(rem-n)};
  }
};

#endif  // DYNAMIC_SQUARE_MATRIX_HPP_
