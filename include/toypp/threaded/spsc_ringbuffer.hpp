#ifndef TOYPP_THREADED_SPSC_RINGBUFFER_HPP_
#define TOYPP_THREADED_SPSC_RINGBUFFER_HPP_

#include <atomic>
#include <memory>
#include <cstring>

namespace tpp {

/**
 * Thread-Safe wait-free single-procucer-single-consumer continugous ring buffer of bytes.
 * 
 * `read` (consumer) can be used by only one thread at a time.
 * `write` (producer) can be used by only one thread at a time.
 * `read` and `write` can be used in two different threads simultanously.
 */
class SPSCRingBuffer {
  std::size_t buffer_size_ = 0;
  std::unique_ptr<char[]> buffer_;
  std::atomic<std::size_t> head_{0};
  std::atomic<std::size_t> tail_{0};

 public:
  SPSCRingBuffer(std::size_t size)
    : buffer_size_(size)
    , buffer_(std::make_unique<char[]>(size)) 
  {}
  SPSCRingBuffer(const SPSCRingBuffer&) = delete;
  SPSCRingBuffer(SPSCRingBuffer&&) noexcept = delete;
  SPSCRingBuffer& operator=(const SPSCRingBuffer&) = delete;
  SPSCRingBuffer& operator=(SPSCRingBuffer&&) noexcept = delete;
  ~SPSCRingBuffer() = default;

  auto read(char* ptr, std::size_t size) -> std::size_t
  {
    const auto head = head_.load();
    const auto tail = tail_.load();
    if (head == tail) {
        return 0;
    }
    const auto readable_size = (head <= tail) ? (tail - head) : (buffer_size_ - head);
    const auto read_count = std::min(size, readable_size);
    std::memcpy(ptr, buffer_.get() + head, read_count);
    head_ = (head <= tail) ? tail : 0;
    return read_count;
  }

  auto write(const char* ptr, std::size_t size) -> std::size_t
  {
    const auto head = head_.load();
    const auto tail = tail_.load();
    if (buffer_size_ == (head + tail - 1)) {
        return 0;
    }
    const auto writable_size = (head <= tail) ? (buffer_size_ - tail) : (head - tail);
    const auto write_count = std::min(size, writable_size);
    std::memcpy(buffer_.get() + tail, ptr, write_count);
    tail_ = (head <= tail) ? 0 : head - 1;
    return write_count;
  }
};

}  // namespace tpp

#endif  // TOYPP_THREADED_SPSC_RINGBUFFER_HPP_
