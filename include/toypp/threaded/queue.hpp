#ifndef TOYPP_THREADED_QUEUE_HPP_
#define TOYPP_THREADED_QUEUE_HPP_

#include <type_traits>
#include <optional>
#include <mutex>
#include <atomic>
#include <thread>
#include <utility>

namespace tpp {

template <typename T>
class MTQueue {
 public:
  using value_type = std::remove_cv_t<std::remove_reference_t<T>>;

 private:
  struct Node {
    value_type data{};
    Node* next = nullptr;
  };

  std::mutex mutex_;
  Node* head_ = nullptr;
  Node* tail_ = nullptr;
  std::atomic<std::size_t> size_ = 0;

 public:
  MTQueue() {}
  MTQueue(const MTQueue&) = delete;
  MTQueue(MTQueue&&) noexcept = delete;
  MTQueue& operator=(const MTQueue&) = delete;
  MTQueue& operator=(MTQueue&&) noexcept = delete;
  ~MTQueue()
  {
    clear();
  }

  [[nodiscard]] auto size() const noexcept -> std::size_t
  {
    return size_;
  }

  void clear()
  {
    std::lock_guard lk(mutex_);
    size_ = 0;
    tail_ = nullptr;
    while (head_) {
      const auto next = head_->next;
      delete head_;
      head_ = next;
    }
  }

  void push(const value_type& obj)
  {
    Node* node = new Node{obj, nullptr};
    std::lock_guard lk(mutex_);
    push_node_unsafe(node);
  }

  void push(value_type&& obj)
  {
    Node* node = new Node{std::move(obj), nullptr};
    std::lock_guard lk(mutex_);
    push_node_unsafe(node);
  }

  [[nodiscard]] auto pop() -> std::optional<value_type>
  {
    if (size_ == 0) {  // empty
      return std::nullopt;
    }

    std::optional<value_type> ret;

    Node* node_to_delete = nullptr;
    {
      std::lock_guard lk(mutex_);
      if (!head_) {  // empty
        return std::nullopt;
      }

      ret = std::move(head_->data);
      node_to_delete = head_;
      head_ = head_->next;
      if (!head_) {  // it was the last node
        tail_ = nullptr;
      }
      --size_;
    }
    delete node_to_delete;

    return ret;
  }

 private:
  void push_node_unsafe(Node* node)
  {
    if (!tail_) {  // empty
      head_ = tail_ = node;
      ++size_;
      return;
    }

    tail_->next = node;
    tail_ = node;
    ++size_;
  }
};

}  // namespace tpp

#endif  // TOYPP_THREADED_QUEUE_HPP_
