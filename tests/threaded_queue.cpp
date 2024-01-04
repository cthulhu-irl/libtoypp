#include <thread>

#include <catch2/catch_all.hpp>

#include "toypp/threaded/queue.hpp"

TEST_CASE("tpp::MTQueue") {
  SECTION("push-pop") {
    tpp::MTQueue<int> queue{};

    CHECK(queue.size() == 0);

    queue.push(1);
    queue.push(2);
    queue.push(3);

    CHECK(queue.size() == 3);
    REQUIRE(queue.pop() == 1);

    CHECK(queue.size() == 2);
    REQUIRE(queue.pop() == 2);

    CHECK(queue.size() == 1);
    REQUIRE(queue.pop() == 3);
  
    CHECK(queue.size() == 0);
    REQUIRE(queue.pop() == std::nullopt);
  }

  SECTION("single-producer-single-consumer") {
    constexpr std::size_t count_max = 10'000;
    tpp::MTQueue<std::size_t> queue;

    std::thread producer([&] {
      for (std::size_t i = 0; i < count_max; ++i) {
        queue.push(i);
      }
    });

    std::thread consumer([&] {
      std::optional<std::size_t> res;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      for (std::size_t i = 0; i < count_max; ++i) {
        auto res = queue.pop();
        // when consumer is faster, try stalling so producer would catch up.
        for (int x = 0; !res && x < 4; ++x) {  
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          res = queue.pop();
        }
        REQUIRE(res == i);
      }
    });

    producer.join();
    consumer.join();
  }

  SECTION("multi-producer-multi-consumer") {
    tpp::MTQueue<std::size_t> queue;
    constexpr std::size_t count_max = 1'000;

    std::atomic<std::size_t> producer_sum{0};
    std::atomic<std::size_t> consumer_sum{0};

    auto producer = [&] {
      for (int n = 0; n < count_max; ++n) {
        queue.push(n);
        producer_sum += n;
      }
    };

    auto consumer = [&] {
      std::optional<std::size_t> res;
      do {
        res = queue.pop();
        // when consumer is faster, try stalling so producer would catch up.
        for (int x = 0; !res && x < 4; ++x) {  
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          res = queue.pop();
        }
        consumer_sum += res.value_or(0);
      } while (res);
    };

    std::vector<std::thread> producer_threads;
    std::vector<std::thread> consumer_threads;

    for (int i = 0; i < 4; ++i) {
        producer_threads.emplace_back(producer);
        consumer_threads.emplace_back(consumer);
    }

    for (auto& thread : producer_threads) {
        thread.join();
    }
    for (auto& thread : consumer_threads) {
        thread.join();
    }

    REQUIRE(producer_sum == consumer_sum);
  }
}
