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

  static_assert(
      !std::is_constructible<
        tpp::UniquePtr<int, MyDeleter<int>&>, int*, MyDeleter<int>&&>::value,
      "rvalue deleter ref for non-const Deleter ref must be deleted."
  );

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

  static_assert(
      std::is_constructible<decltype(a), decltype(std::move(a))>::value);
  static_assert(
      std::is_constructible<decltype(b), decltype(std::move(b))>::value);
  static_assert(
      std::is_constructible<decltype(c), decltype(std::move(c))>::value);
  static_assert(
      std::is_constructible<decltype(d), decltype(std::move(d))>::value);
  static_assert(
      std::is_constructible<decltype(e), decltype(std::move(e))>::value);
}
