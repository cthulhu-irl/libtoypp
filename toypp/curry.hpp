#ifndef TOYPP_CURRY_HPP_
#define TOYPP_CURRY_HPP_

#include <functional>
#include <tuple>
#include <utility>
#include <cstdint>

namespace tpp {

namespace detail {

  template <typename ...Ts>
  struct Expandable {
    template <template <typename...> typename Consumer, typename ...Us>
    using expand_for_t = Consumer<Ts..., Us...>;

    template <template <typename...> typename Consumer, typename ...Us>
    using rexpand_for_t = Consumer<Us..., Ts...>;
  };

  template <typename ...Ts>
  struct TypeInfo : public Expandable<Ts...> {
    constexpr static auto size = sizeof...(Ts);
  };

  template <typename T, typename ...Ts>
  struct TypeInfo<T, Ts...> : public Expandable<T, Ts...> {
    using first = T;
    constexpr static auto rest = TypeInfo<Ts...>{};

    constexpr static auto size = sizeof...(Ts);
  };


  template <typename ...TypeInfos>
  struct MixTypeInfo {};

  template <>
  struct MixTypeInfo<> { using type = TypeInfo<>; };

  template <typename TI>
  struct MixTypeInfo<TI> { using type = TI; };

  template <typename TI, typename ...TypeInfos>
  struct MixTypeInfo<TI, TypeInfos...> {

    template <typename ...Ts>
    struct mixer {
      template <typename ...Us>
      struct inner {
        using type = TypeInfo<Ts..., Us...>;
      };
    };

    using mixer_type = typename TI::template expand_for_t<mixer>;

    using type =
      typename MixTypeInfo<TypeInfos...>
        ::type::template expand_for_t<mixer_type::template inner>::type;
  };

  template <typename ...Ts>
  using mix_typeinfo_t = typename MixTypeInfo<Ts...>::type;


  template <std::size_t I, typename ...Ts>
  struct PackDrop { using type = TypeInfo<Ts...>; };

  template <std::size_t I, typename T, typename ...Ts>
  struct PackDrop<I, T, Ts...> {
    using type = 
      typename std::conditional<
        I == 0,
          TypeInfo<T, Ts...>,
          typename PackDrop<I-1, Ts...>::type
      >::type;
  };


  template <typename Func,
            typename Ret, typename SuppliedTypeInfo, typename ...Args>
  class Curry {
   public:
    using ret_type = Ret;
    constexpr static auto args_type_info = TypeInfo<Args...>{};

    constexpr static uint8_t arity = sizeof...(Args);

    using function_type = Func;
    
    using holder_type =
      typename SuppliedTypeInfo::template expand_for_t<std::tuple>;

    template <typename ...Ts>
    using supply_curry_type =
      typename PackDrop<sizeof...(Ts), Args...>::type
        ::template rexpand_for_t<
          Curry,
          Func,
          Ret,
          typename SuppliedTypeInfo
            ::template expand_for_t<TypeInfo, Ts...>
        >;

   private:
    function_type func_;
    holder_type   args_;

   public:
    template <typename F>
    constexpr Curry(F&& callable) : func_(std::forward<F>(callable)) {}

    template <typename T, typename ...Ts>
    constexpr auto operator()(T&& first, Ts&&... rest) &
    {
      return supply_curry_type<T, Ts...>(
        func_,
        std::tuple_cat(
          args_,
          std::forward_as_tuple(
            std::forward<T>(first),
            std::forward<Ts>(rest)...
          )
        )
      );
    }

    template <typename T, typename ...Ts>
    constexpr auto operator()(T&& first, Ts&&... rest) const&
    {
      return supply_curry_type<T, Ts...>(
        func_,
        std::tuple_cat(
          args_,
          std::forward_as_tuple(
            std::forward<T>(first),
            std::forward<Ts>(rest)...
          )
        )
      );
    }

    template <typename T, typename ...Ts>
    constexpr auto operator()(T&& first, Ts&&... rest) &&
    {
      return supply_curry_type<T, Ts...>(
        std::move(func_),
        std::tuple_cat(
          std::move(args_),
          std::forward_as_tuple(
            std::forward<T>(first),
            std::forward<Ts>(rest)...
          )
        )
      );
    }

    template <typename T, typename ...Ts>
    constexpr auto operator()(T&& first, Ts&&... rest) const&&
    {
      return supply_curry_type<T, Ts...>(
        std::move(func_),
        std::tuple_cat(
          std::move(args_),
          std::forward_as_tuple(
            std::forward<T>(first),
            std::forward<Ts>(rest)...
          )
        )
      );
    }

    constexpr Ret operator()(Args... args)
    {
      if constexpr (sizeof...(Args) == 0)
        return std::apply(func_, args_);

      else
        return std::apply(
          func_,
          std::tuple_cat(
            args_,
            std::forward_as_tuple(std::forward<Args>(args)...)
          )
        );
    }

    constexpr Ret operator()(Args... args) const
    {
      if constexpr (sizeof...(Args) == 0)
        return std::apply(func_, args_);

      return std::apply(
        func_,
        std::tuple_cat(
          args_,
          std::forward_as_tuple(std::forward<Args>(args)...)
        )
      );
    }

   //private:
    template <typename F>
    constexpr Curry(F&& callable, const holder_type& args)
      : func_(std::forward<F>(callable))
      , args_(args)
    {}

    template <typename F>
    constexpr Curry(F&& callable, holder_type&& args)
      : func_(std::forward<F>(callable))
      , args_(std::move(args))
    {}
  };

}  // namespace detail

template <typename Ret, typename ...Args>
auto curry(Ret(&func)(Args...))
{
  return detail::Curry<decltype(func),
                       Ret, detail::TypeInfo<>, Args...>(func);
}

};

#endif  // TOYPP_CURRY_HPP_
