#ifndef TOYPP_FLATMAP_HPP_
#define TOYPP_FLATMAP_HPP_

#include <iterator>
#include <type_traits>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <optional>
#include <utility>
#include <stdexcept>

namespace tpp {

template <typename Key,
          typename Value,
          typename Compare = std::less<Key>,
          typename Container = std::vector<std::pair<Key, Value>>>
class FlatMap {
  Container container_{};

  struct SearchResult final { bool found = false; std::size_t index = 0; };

 public:
  using key_type = Key;
  using mapped_type = Value;
  using value_type = typename Container::value_type;

  constexpr FlatMap() noexcept {}

  constexpr void reserve(std::size_t n) { container_.reserve(n); }

  constexpr Value* at(const Key& key) noexcept {
    const auto res = binary_search(container_, key);
    if (!res.found) return nullptr;
    return std::addressof(container_[res.index].second);
  }

  constexpr const Value* at(const Key& key) const noexcept {
    const auto res = binary_search(container_, key);
    if (!res.found) return nullptr;
    return std::addressof(container_[res.index].second);
  }

  constexpr bool insert(Key key, Value value, bool can_override = false) {
    const auto res = binary_search(container_, key);
    if (res.found) {
      if (!can_override) return false;
      container_[res.index].second = std::move(value);
      return true;
    }

    container_.emplace_back(std::move(key), std::move(value));

    // rotate to the right by 1.
    std::rotate(std::begin(container_) + res.index,
                std::begin(container_) + (std::size(container_) - 1),
                std::end(container_));

    return true;
  }

  constexpr bool insert_or_assign(Key key, Value value) {
    return insert(std::move(key), std::move(value), true);
  }

  constexpr bool remove(const Key& key) {
    const auto res = binary_search(container_, key);
    if (!res.found) return false;

    // rotate to the left by 1.
    std::rotate(std::begin(container_) + res.index,
                std::begin(container_) + res.index + 1,
                std::end(container_));

    container_.pop_back();

    return true;
  }

  constexpr Value& operator[](const Key& key) noexcept {
    const auto res = binary_search(container_, key);
    if (!res.found)
      insert(key, Value{});

    return container_[res.index].second;
  }

  constexpr const Value& operator[](const Key& key) const noexcept {
    return at(key);
  }

 private:
  template <typename C, typename T>
  static constexpr SearchResult binary_search(C&& container, T&& value)
  {
    if (std::size(container) == 0) return {};

    Compare compare{};

    // due unsigned overflow problem when calculating `mid`...
    if (compare(value, container[0].first)) return {false, 0};

    std::size_t low = 0;
    std::size_t high = std::size(container) - 1;

    std::size_t mid = 0;  
    while (low <= high) {
      mid = low + (high - low) / 2;

      if (value == container[mid].first) return {true, mid};

      if (compare(container[mid].first, value)) low  = mid + 1;
      else                                      high = mid - 1;
    }
  
    return {false, mid};
  }
};

}  // namespace tpp

#endif  // TOYPP_FLATMAP_HPP_
