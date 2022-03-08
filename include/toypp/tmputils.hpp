#ifndef TOYPP_TMPUTILS_HPP_
#define TOYPP_TMPUTILS_HPP_

#include <cstdint>
#include <type_traits>

namespace tpp {

// -- basic tag types

struct not_found_t {};

// -- identity_type / identity_value

template <typename T>
struct identity_type { using type = T; };

template <auto X>
struct identity_value { constexpr static auto value = X; };

template <typename T>
using identity_t = typename identity_type<T>::type;

template <auto X>
constexpr inline auto identity_v = X;

// -- always_type / always_value

template <typename T, typename U>
struct always_type { using type = T; };

template <auto X, auto Y>
struct always_value { constexpr static auto value = X; };

template <typename T, typename U>
using always_t = typename always_type<T, U>::type;

template <auto X, auto Y>
constexpr inline auto always_v = X;

// -- unalias_template

template <template <typename ...> typename Template, typename ...Ts>
struct unalias_template {
  template <typename ...Us>
  struct unaliased { using type = Template<Ts..., Us...>; };

  template <typename ...Us>
  struct unaliased_r { using type = Template<Ts..., Us...>; };

  template <typename ...Us>
  using unaliased_r_t = typename unaliased<Us...>::type;

  template <typename ...Us>
  using unaliased_t = typename unaliased<Us...>::type;

  using type = Template<Ts...>;
};

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

// -- type_pack_mix

template <typename ...Packs>
struct type_pack_mix {
  using type = type_pack<>;
};

template <typename Pack>
struct type_pack_mix<Pack> {
  using type = Pack;
};

template <typename PackA, typename PackB, typename ...Packs>
struct type_pack_mix<PackA, PackB, Packs...> {

  using type = typename PackB::template expand_into<
                  typename PackA::expand_into, type_pack>;
};

template <typename ...Packs>
using type_pack_mix_t = typename type_pack_mix<Packs...>::type;

// -- value_pack

template <auto ...Xs>
struct value_pack {};

template <>
struct value_pack<> {
  using rest = value_pack<>;

  constexpr static auto count = 0;
  constexpr static auto size() noexcept { return count; }

  template <template <auto ...> typename Template, auto ...Ys>
  using expand_into = Template<Ys...>;

  template <template <auto ...> typename Template, auto ...Ys>
  using rexpand_into = Template<Ys...>;
};

template <auto X, auto ...Xs>
struct value_pack<X, Xs...> {
  constexpr static auto first = X;

  using rest = value_pack<Xs...>;

  template <template <auto ...> typename Template, auto ...Ys>
  using expand_into = Template<Xs..., Ys...>;

  template <template <auto ...> typename Template, auto ...Ys>
  using rexpand_into = Template<Ys..., Xs...>;

  constexpr static auto count = sizeof...(Xs);
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
using type_pack_zip_t = typename type_pack_zip<Packs...>::type;

// -- value_pack_zip

namespace detail {

template <auto ...Xs>
struct value_pack_zip {};

template <auto X, auto ...Xs>
struct value_pack_zip<X, Xs...> {
  template <auto ...Ys>
  struct with { using type = value_pack<X, Xs...>; };

  template <auto Y, auto ...Ys>
  struct with<Y, Ys...> {
    using rest = typename value_pack_zip<Xs...>::template with<Ys...>::type;
    using type = typename rest::template rexpand_into<X, Y>;
  };
};

template <typename PackA, typename PackB>
struct value_pack_zip_two {
  using zipper =
    typename PackA::template expand_into<detail::value_pack_zip>;

  using type =
    typename PackB::template expand_into<typename zipper::with>::type;
};

}  // namespace detail

template <typename ...Packs>
struct value_pack_zip {
  using type = value_pack<>;
};

template <typename Pack>
struct value_pack_zip<Pack> { using type = Pack; };

template <typename PackA, typename PackB>
struct value_pack_zip<PackA, PackB> {
  using type = typename detail::value_pack_zip_two<PackA, PackB>::type;
};

template <typename PackA, typename PackB, typename PackC, typename ...Packs>
struct value_pack_zip<PackA, PackB, PackC, Packs...> {
  using tmppack1 = typename detail::value_pack_zip_two<PackA, PackB>::type;
  using tmppack2 = typename detail::value_pack_zip_two<tmppack1, PackC>::type;

  using type = typename value_pack_zip<tmppack2, Packs...>::type;
};

template <typename ...Packs>
using value_pack_zip_t = typename value_pack_zip<Packs...>::type;

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

// -- pack_pick_best

template <template <typename, typename> typename Compare,
          typename ...Ts>
struct pack_pick_best {};

template <template <typename, typename> typename Compare,
          typename T>
struct pack_pick_best<Compare, T> {
  using type = T;
};

template <template <typename, typename> typename Compare,
          typename A,
          typename B,
          typename ...Rest>
struct pack_pick_best<Compare, A, B, Rest...> {
  using best_sofar = typename Compare<A, B>::type;

  using type = typename pack_pick_best<Compare, best_sofar, Rest...>::type;
};

template <template <typename, typename> typename Compare, typename ...Ts>
using pack_pick_best_t = typename pack_pick_best<Compare, Ts...>::type;

// -- pack_filter

template <template <typename> typename Selector,
          typename ...Ts>
struct pack_filter {
  using type = type_pack<>;
};

template <template <typename> typename Selector,
          typename T,
          typename ...Ts>
struct pack_filter<Selector, T, Ts...> {
  using first =
    std::conditional_t<Selector<T>::value, type_pack<T>, type_pack<>>;

  using rest = typename pack_filter<Selector, Ts...>::type;

  using type = typename type_pack_mix<first, rest>::type;
};

template <template <typename> typename Selector, typename ...Ts>
using pack_filter_t = typename pack_filter<Selector, Ts...>::type;

// -- pack_find_match

template <template <typename> typename Pred,
          typename ...Ts>
struct pack_find_match {
  using type = not_found_t;
};

template <template <typename> typename Pred,
          typename T,
          typename ...Ts>
struct pack_find_match<Pred, T, Ts...> {
  using type =
    std::conditional_t<Pred<T>::value,
                       T,
                       typename pack_find_match<Pred, Ts...>::type>;
};

// -- pack_pick_max_size

template <typename ...Ts>
struct pack_pick_max_size {};

template <typename T, typename ...Ts>
struct pack_pick_max_size<T, Ts...> {
  template <typename A, typename B>
  using max_size_type = std::conditional<sizeof(A) >= sizeof(B), A, B>;

  using type = typename pack_pick_best<max_size_type, T, Ts...>::type;
};

template <typename ...Ts>
using pack_pick_max_size_t = typename pack_pick_max_size<Ts...>::type;

// -- pack_commutative_binary_compliance

template <template <typename, typename> typename BinaryPred,
          typename ...Ts>
struct pack_commutative_binary_compliance : std::true_type {};

template <template <typename, typename> typename BinaryPred,
          typename T,
          typename U>
struct pack_commutative_binary_compliance<BinaryPred, T, U>
  : std::bool_constant<BinaryPred<T, U>::value> {};

template <template <typename, typename> typename BinaryPred,
          typename T,
          typename ...Ts>
struct pack_commutative_binary_compliance<BinaryPred, T, Ts...> {
  constexpr static bool value =
    (true && ... && (BinaryPred<T, Ts>::value))
    && pack_commutative_binary_compliance<BinaryPred, Ts...>::value;
};

template <template <typename, typename> typename BinaryPred,
          typename ...Ts>
constexpr inline bool pack_commutative_binary_compliance_v =
  pack_commutative_binary_compliance<BinaryPred, Ts...>::value;

// -- pack_is_unique

template <typename ...Ts>
struct pack_is_unique {
  template <typename A, typename B>
  using is_not_same = std::negation<std::is_same<A, B>>;

  constexpr static bool value =
    pack_commutative_binary_compliance_v<is_not_same, Ts...>;
};

template <typename ...Ts>
constexpr inline bool pack_is_unique_v = pack_is_unique<Ts...>::value;

// -- pack_has_match

template <template <typename> typename Pred, typename ...Ts>
struct pack_has_match {
  constexpr static bool value = (false || ... || Pred<Ts>::value);
};

// -- pack_does_contain

template <typename A, typename ...Ts>
struct pack_does_contain {
  constexpr static bool value = (false || ... || std::is_same_v<A, Ts>);
};

}  // namespace tpp

#endif  // TOYPP_TMPUTILS_HPP_
