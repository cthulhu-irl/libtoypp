#ifndef TOYPP_FUNCTIONAL_HPP_
#define TOYPP_FUNCTIONAL_HPP_

#include <tuple>
#include <utility>

namespace tpp {

template <typename F, typename ...Fs>
constexpr auto compose(F&& f, Fs&& ...fs) {
  if constexpr (sizeof...(Fs) == 0)
    return std::forward<F>(f);
  else
    return [first=std::forward<F>(f),
            rest=std::make_tuple(std::forward<Fs>(fs)...)](auto&& ...args)
    {
      constexpr auto composer = [](auto&& ...fns) {
        return compose(std::forward<decltype(fns)>(fns)...);
      };
      return first(
        std::apply(composer, std::move(rest))(
          std::forward<decltype(args)>(args)...
        )
      );
    };
}

}  // namespace tpp

#endif  // TOYPP_FUNCTIONAL_HPP_
