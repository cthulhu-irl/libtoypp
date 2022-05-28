#ifndef TOYPP_THREADED_THREADPOOL_HPP_
#define TOYPP_THREADED_THREADPOOL_HPP_

#include <cstddef>
#include <atomic>
#include <vector>
#include <queue>
#include <functional>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace tpp {

class ThreadPool {
  using task_type = std::function<void()>;

  std::vector<std::thread> workers_;
  std::mutex               mutex_; // restrict access to queue.
  std::condition_variable  cv_; // wait and notify on new task.
  std::queue<task_type>    queue_;
  std::atomic<bool>        halted_{false};
  std::atomic<bool>        shutdowned_{false};

  void worker_loop() {
    while (true) {
      task_type task;

      {
        std::unique_lock<std::mutex> lock{mutex_};
        cv_.wait(lock, [this] { return !queue_.empty(); });

        if (shutdowned_.load(std::memory_order_relaxed)) return;
        if (!queue_.front()) return; // poison pill.

        task = std::move(queue_.front());
        queue_.pop();
      }

      task();
    }
  }

 public:
  ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

  ThreadPool(std::size_t size) {
    if (size == 0)
      size = std::thread::hardware_concurrency();

    workers_.reserve(size);

    for (std::size_t i = 0; i < size; ++i)
      workers_.emplace_back(&ThreadPool::worker_loop, this);
  }

  ThreadPool(const ThreadPool&) = delete;

  ~ThreadPool() {
    if (shutdowned_.load(std::memory_order_relaxed))
      return;

    if (!halted_.load(std::memory_order_relaxed))
      shutdown();
  }

  bool running() const noexcept {
    return !halted_.load(std::memory_order_relaxed);
  }

  std::size_t workers_count() const noexcept { return workers_.size(); }

  std::size_t jobs_count() noexcept {
    std::lock_guard<std::mutex> lock{mutex_};
    return queue_.size();
  }

  template <typename F>
  void add_task(F&& task) {
    {
      std::lock_guard<std::mutex> lock{mutex_};
      queue_.emplace(std::forward<F>(task));
    }
    cv_.notify_one();
  }

  /// graceful shutdown; lets workers do all the tasks in queue so far.
  void shutdown() {
    halted_.store(true, std::memory_order_relaxed);

    {
      std::lock_guard<std::mutex> lock{mutex_};
      queue_.emplace(task_type{}); // aka poison pill.
    }
    cv_.notify_all();

    for (auto& worker : workers_)
      worker.join();

    shutdowned_.store(true, std::memory_order_relaxed);
    workers_.clear();
  }

  /// forces workers to leave queued tasks and stop when current task is done.
  void force_shutdown() {
    shutdowned_.store(true, std::memory_order_relaxed);
    shutdown();
  }
};

}  // namespace tpp

#endif  // TOYPP_THREADED_THREADPOOL_HPP_
