#include <memory>
#include <type_traits>
#include <utility>

#include <catch2/catch.hpp>
#include <toypp/uniqueptr.hpp>

template <typename T>
class MyDeleter final {
  using pointer = std::add_pointer_t<std::remove_reference_t<T>>;

  pointer ptr_;

 public:
  constexpr MyDeleter(pointer ptr) noexcept : ptr_(ptr) {}

  void operator()(pointer ptr) const noexcept
  {
    REQUIRE(ptr == ptr_);
  }
};

TEST_CASE("[tpp::uniqueptr]")
{
  int val = 42;
  int* valptr = std::addressof(val);
  MyDeleter<int> my_deleter{valptr};

  tpp::UniquePtr<int> a{};
  tpp::UniquePtr<int> b(new int(12));
  tpp::UniquePtr<int, MyDeleter<int>> c(valptr, my_deleter);
  tpp::UniquePtr<int, MyDeleter<int>&> d(valptr, my_deleter);
  tpp::UniquePtr<int, const MyDeleter<int>&> e(valptr, my_deleter);

// this must yield error of call to deleted constructor:
//  tpp::UniquePtr<int, MyDeleter<int>&> f(valptr, std::move(my_deleter));

  tpp::UniquePtr<int, MyDeleter<int>> g(valptr, std::move(my_deleter));

  tpp::UniquePtr<int> x = tpp::make_unique<int>(24);
  tpp::UniquePtr<int> y(nullptr);

  REQUIRE_FALSE(a);
  REQUIRE_FALSE(y);

  REQUIRE(*b == 12);
  REQUIRE(*c == val);
  REQUIRE(*d == val);
  REQUIRE(*e == val);
  REQUIRE(*g == val);
  REQUIRE(*x == 24);
}
