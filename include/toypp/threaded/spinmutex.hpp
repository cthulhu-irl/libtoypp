#ifndef TOYPP_THREADED_SPINMUTEX_HPP_
#define TOYPP_THREADED_SPINMUTEX_HPP_

#include <atomic>

namespace tpp {

class SpinMutex {
  std::atomic<bool> flag_{false};

 public:
  SpinMutex() {}
  SpinMutex(const SpinMutex&) = delete;

  void acquire() noexcept {
    bool current = false;
    while (!flag_.compare_exchange_weak(current, true))
      current = false;
  }

  void release() noexcept { flag_.store(false); }
};

}  // namespace tpp

#endif  // TOYPP_THREADED_SPINMUTEX_HPP_
