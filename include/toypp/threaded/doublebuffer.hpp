#ifndef TOYPP_THREADED_DOUBLEBUFFER_HPP_
#define TOYPP_THREADED_DOUBLEBUFFER_HPP_

#include <atomic>
#include <mutex>

namespace tpp {

/**
 * @brief A generic DoubleBuffer implmentation to segregate
 *        read and write on available and back buffers.
 * 
 * DoubleBuffer is often used as a single item queue, where
 * one thread bakes its result into offline buffer, and when
 * ready, swaps it to become the available buffer,
 * so another can read it in parallel, and meanwhile
 * next result will be in write.
 * 
 * @tparam BufferT buffer type.
 */
template <typename BufferT>
class DoubleBuffer {
 public:
  using buffer_type = std::remove_reference_t<BufferT>;

 private:
  buffer_type buffers_[2] = {};
  std::size_t buffer_reader_index_ = 1;

  std::atomic<bool> flag_{false};  // false => sync'ed, true => awaiting.

 public:
  DoubleBuffer() {}

  DoubleBuffer(buffer_type a, buffer_type b)
    : buffers_{ std::move(a), std::move(b) }
  {}

  void reader_arrive() noexcept {
    bool expected = true;
    if (flag_.compare_exchange_weak(expected, false)) {
      buffer_reader_index_ = (buffer_reader_index_ + 1) & 1;
    }
  }

  void writer_arrive() noexcept {
    // indicate
    flag_ = true;
    // wait
    bool expected = false;
    while (!flag_.compare_exchange_weak(expected, true)) {
      expected = false;
    }
  }

  buffer_type& reader_buffer() noexcept {
    return buffers_[buffer_reader_index_ & 1];
  }

  const buffer_type& reader_buffer() const noexcept {
    return buffers_[buffer_reader_index_ & 1];
  }

  buffer_type& writer_buffer() noexcept {
    return buffers_[(buffer_reader_index_ + 1) & 1];
  }

  const buffer_type& writer_buffer() const noexcept {
    return buffers_[(buffer_reader_index_ + 1) & 1];
  }
};

}  // namespace tpp

#endif  // TOYPP_THREADED_DOUBLEBUFFER_HPP_

