#ifndef TOYPP_THREADED_SEMAPHORE_HPP_
#define TOYPP_THREADED_SEMAPHORE_HPP_

#include <atomic>
#include <thread>

namespace tpp {

template <std::size_t MaxCount = 0>
class Semaphore {
 public:
  static const std::size_t max;

 private:
  std::atomic<int> count_;

 public:
  Semaphore() : count_(max) {}
  Semaphore(const Semaphore&) = delete;

  void acquire() noexcept {
    auto current = count_.load();
    while ((current += (current == 0)) // if current == 0: current = 1
          && !count_.compare_exchange_strong(current, current-1));
  }

  void release() noexcept { count_.fetch_add(1); }
};

template <std::size_t MaxCount>
const std::size_t Semaphore<MaxCount>::max =
    MaxCount ? MaxCount : std::thread::hardware_concurrency();

}  // namespace tpp

#endif  // TOYPP_THREADED_SEMAPHORE_HPP_
