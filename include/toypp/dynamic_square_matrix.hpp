#ifndef TOYPP_DYNAMIC_SQUARE_MATRIX_HPP_
#define TOYPP_DYNAMIC_SQUARE_MATRIX_HPP_

#include <cstddef>
#include <cmath>
#include <iterator>
#include <utility>
#include <vector>

namespace tpp {

/** A Dynamically Allocated Only Square Shape Matrix
 *  that is suitable for resizing so much...
 *
 *  NOTE: its linear indexing is different, see `index` private function.
 */
template <typename T, typename Vector = std::vector<T>>
class DynamicSquareMatrix {
  Vector      vec_;
  std::size_t size_ = 0;

 public:
  using value_type = T;

  constexpr explicit DynamicSquareMatrix() {}
  constexpr explicit DynamicSquareMatrix(std::size_t n) { resize(n); }

  constexpr std::size_t size() const noexcept { return size_; }

  constexpr void resize(std::size_t n, T value = T())
  {
    vec_.resize(n * n, std::move(value));
    size_ = n;
  }

  constexpr T& at(std::size_t n, std::size_t m)
  {
    // assert(size() <= n || size() <= m);

    return vec_[index(n, m)];
  }

  constexpr const T& at(std::size_t n, std::size_t m) const
  {
    // assert(size() <= n || size() <= m);

    return vec_[index(n, m)];
  }

  constexpr void add_rowcol(std::size_t count, T value = T())
  {
    resize(size() + count, std::move(value));
  }

  constexpr void pop_rowcol(std::size_t x)
  {
    std::size_t base = x * x; // index(0, x)
    // L is 2*x + intersectioned 1
    // also for each rowcol there are 2 removals: index(n, x) and index(x, n)
    std::size_t delcount = (2*x + 1) + 2*(size() - (x+1));
    std::size_t end = vec_.size() - delcount;

    std::size_t srcidx = (x+1)*(x+1) - 1 + (x == 0); // index(0, x+1) - 1

    std::size_t n = x;

    for (std::size_t dstidx = base; dstidx < end; ++dstidx) {
      n += dstidx >= (n*n); // current rowcol

      // skip index(n, x) or index(x, n)
      ++srcidx;
      srcidx += (srcidx == (n*n + x)) | (srcidx == (n*n + n + x));

      vec_[dstidx] = std::move(vec_[srcidx]);
    }

    resize(size() - 1);
  }

  // NOTE: the iteration order won't be same as other matrices...
  // idx != (i * width + j)
  // idx == max(i,j)*max(i,j) + (max(i,j) == j) * i + j

  constexpr auto begin() { return std::begin(vec_); }
  constexpr auto end()   { return std::end(vec_);   }

  constexpr const auto begin() const { return std::begin(vec_); }
  constexpr const auto end()   const { return std::end(vec_);   }

  constexpr const auto cbegin() const { return std::cbegin(vec_); }
  constexpr const auto cend()   const { return std::cend(vec_);   }

  constexpr auto operator()(std::size_t n, std::size_t m)
  {
    return at(n, m);
  }

  constexpr auto operator()(std::size_t n, std::size_t m) const
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

}  // namespace tpp

#endif  // TOYPP_DYNAMIC_SQUARE_MATRIX_HPP_
