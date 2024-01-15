#include <catch2/catch_all.hpp>

#include "toypp/span.hpp"

TEST_CASE("tpp::Span") {
    std::array<int, 3> arr = {1, 2, 3};
    tpp::Span<int> span(arr.data(), arr.size());

    CHECK(span.data() == arr.data());
    CHECK(span.size() == arr.size());
    CHECK(span.size_bytes() == arr.size()*sizeof(int));

    CHECK(span[0] == 1);
    CHECK(span[1] == 2);
    CHECK(span[2] == 3);

    CHECK(span.subspan(1).size() == arr.size()-1);
    CHECK(span.subspan(1).data() == arr.data()+1);
    CHECK(span.subspan(1, 1).size() == 1);
    CHECK(span.subspan(1, 1).data() == arr.data()+1);

    CHECK(span.front() == 1);
    CHECK(span.back() == 3);

    CHECK(!span.empty());
    CHECK(span.subspan(3).empty());

    span[0] = 42;
    CHECK(arr[0] == 42);

    tpp::Span<const int> const_span = span;
    CHECK(span.data() == const_span.data());
    CHECK(span.size() == const_span.size());
}
