#include <catch2/catch_all.hpp>

#include "toypp/queue.hpp"

TEST_CASE("tpp::Queue") {
  SECTION("push-pop") {
    tpp::Queue<int> queue{};

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

  SECTION("copy-move") {
    tpp::Queue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);

    auto other = queue;
    REQUIRE(other.pop() == 1);
    REQUIRE(other.pop() == 2);
    REQUIRE(queue.pop() == 1);

    other.push(42);

    queue = other;
    REQUIRE(other.pop() == 3);
    REQUIRE(other.pop() == 42);
    REQUIRE(queue.pop() == 3);
    
    auto another = std::move(queue);
    other = std::move(another);
    REQUIRE(other.pop() == 42);
  }
}
