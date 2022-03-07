#ifndef TOYPP_TMPUTILS_HPP_
#define TOYPP_TMPUTILS_HPP_

#include <cstdint>

namespace tpp {

// -- type_pack

template <typename ...Ts>
struct type_pack {};

template <>
struct type_pack<> {
  using rest = type_pack<>;

  constexpr static auto count = 0;
  constexpr static auto size() noexcept { return count; }

  template <template <typename ...> typename Template, typename ...Us>
  using expand_into = Template<Us...>;

  template <template <typename ...> typename Template, typename ...Us>
  using rexpand_into = Template<Us...>;

  template <typename ...Us>
  using zip_with = type_pack<Us...>;
};

template <typename T, typename ...Ts>
struct type_pack<T, Ts...> {
  using first = T;
  using rest  = type_pack<Ts...>;

  template <template <typename ...> typename Template, typename ...Us>
  using expand_into = Template<Ts..., Us...>;

  template <template <typename ...> typename Template, typename ...Us>
  using rexpand_into = Template<Us..., Ts...>;

  constexpr static auto count = sizeof...(Ts);
  constexpr static auto size() noexcept { return count; }
};

// -- type_pack_zip

namespace detail {

template <typename ...Ts>
struct pack_zip {};

template <typename T, typename ...Ts>
struct pack_zip<T, Ts...> {
  template <typename ...Us>
  struct with { using type = type_pack<T, Ts...>; };

  template <typename U, typename ...Us>
  struct with<U, Us...> {
    using rest = typename pack_zip<Ts...>::template with<Us...>::type;
    using type = typename rest::template rexpand_into<T, U>;
  };
};

template <typename PackA, typename PackB>
struct type_pack_zip_two {
  using zipper = typename PackA::template expand_into<detail::pack_zip>;
  using type =
    typename PackB::template expand_into<typename zipper::with>::type;
};

}  // namespace detail

template <typename ...Packs>
struct type_pack_zip {
  using type = type_pack<>;
};

template <typename Pack>
struct type_pack_zip<Pack> { using type = Pack; };

template <typename PackA, typename PackB>
struct type_pack_zip<PackA, PackB> {
  using type = typename detail::type_pack_zip_two<PackA, PackB>::type;
};

template <typename PackA, typename PackB, typename PackC, typename ...Packs>
struct type_pack_zip<PackA, PackB, PackC, Packs...> {
  using tmppack1 = typename detail::type_pack_zip_two<PackA, PackB>::type;
  using tmppack2 = typename detail::type_pack_zip_two<tmppack1, PackC>::type;

  using type = typename type_pack_zip<tmppack2, Packs...>::type;
};

template <typename ...Packs>
using type_pack_zip_t = typename type_pack<Packs...>::type;

// -- pack_pick_first

template <typename T, typename ...Ts>
struct pack_pick_first { using type = T; };

template <typename ...Ts>
using pack_pick_first_t = typename pack_pick_first<Ts...>::type;

// -- pack_pick_last

template <typename ...Ts>
struct pack_pick_last {};

template <typename T, typename ...Ts>
struct pack_pick_last<T, Ts...> {
  using type = typename pack_pick_last<Ts...>::type;
};

template <typename T>
struct pack_pick_last<T> { using type = T; };

template <typename ...Ts>
using pack_pick_last_t = typename pack_pick_last<Ts...>::type;

// -- pack_pick_idx

template <std::size_t I, typename ...Ts>
struct pack_pick_idx {};

template <std::size_t I, typename T, typename ...Ts>
struct pack_pick_idx<I, T, Ts...> {
  using type = typename pack_pick_idx<I-1, Ts...>::type;
};

template <typename T, typename ...Ts>
struct pack_pick_idx<0, T, Ts...> { using type = T; };

template <std::size_t I, typename ...Ts>
using pack_pick_idx_t = typename pack_pick_idx<I, Ts...>::type;

// -- pack_count

template <typename ...Ts>
struct pack_count { constexpr static auto value = sizeof...(Ts); };

template <typename ...Ts>
constexpr inline auto pack_count_v = sizeof...(Ts);

}  // namespace tpp

#endif  // TOYPP_TMPUTILS_HPP_
