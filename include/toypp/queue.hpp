#ifndef TOYPP_QUEUE_HPP_
#define TOYPP_QUEUE_HPP_

#include <type_traits>
#include <optional>
#include <utility>

namespace tpp {

template <typename T>
class Queue {
 public:
  using value_type = std::remove_cv_t<std::remove_reference_t<T>>;

 private:
  struct Node {
    value_type data;
    Node* next = nullptr;
  };

  std::size_t size_ = 0;
  Node* head_ = nullptr;
  Node* tail_ = nullptr;

 public:
  Queue() {}

  Queue(const Queue& other)
  {
    auto head = other.head_;
    while (head) {
      push(head->data);
      head = head->next;
    }
  }

  Queue(Queue&& other) noexcept
    : head_(std::exchange(other.head_, nullptr))
    , tail_(std::exchange(other.tail_, nullptr))
  {}

  Queue& operator=(const Queue& other)
  {
    if (this == &other) {
      return *this;
    }

    clear();

    auto head = other.head_;
    while (head) {
      push(head->data);
      head = head->next;
    }

    return *this;
  }

  Queue& operator=(Queue&& other) noexcept
  {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(size_, other.size_);
    return *this;
  }

  ~Queue() { clear(); }

  [[nodiscard]] auto size() const noexcept -> std::size_t
  {
    return size_;
  }

  [[nodiscard]] auto empty() const noexcept
  {
    return size_ == 0;
  }

  void clear()
  {
    size_ = 0;
    tail_ = nullptr;
    while (head_) {
      const auto node = head_;
      head_ = head_->next;
      delete node;
    }
  }

  void push(const value_type& item)
  {
    return push_node(new Node{item});
  }

  void push(value_type&& item)
  {
    return push_node(new Node{std::move(item)});
  }

  [[nodiscard]] auto pop() -> std::optional<value_type>
  {
    if (head_ == nullptr) {
        return std::nullopt;
    }

    auto node = head_;
    head_ = head_->next;
    if (!head_) {
        tail_ = nullptr;
    }

    std::optional<value_type> ret = std::move(node->data);
    delete node;
    --size_;
  
    return ret;
  }

 private:
  void push_node(Node* node)
  {
    if (tail_ == nullptr) {
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

#endif  // TOYPP_QUEUE_HPP_
