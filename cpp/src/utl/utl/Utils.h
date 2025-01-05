#pragma once

#include "Base.h"

#include <ranges>
#include <numeric>
#include <sstream>
#include <unordered_set>
#include <algorithm>

namespace utl {

  template<typename F> requires std::invocable<F, std::ostream&>
  constexpr string streamToString(F f) {
    std::stringstream ss;
    f(ss);
    return ss.str();
  }

  template<typename T, typename F>
  constexpr bool evalAllOr(const T& iterable, const F& f) {
    bool result = false;
    for (auto& x : iterable) {
      result |= f(x);
    }
    return result;
  }

  template<typename T, typename S>
    requires std::convertible_to<decltype(S().insert(T()).second), bool>
  constexpr bool hasAndAdd(S& s, const T& e) {
    return !s.insert(e).second;
  }

  template<typename Iterable, typename T>
  constexpr void fill(Iterable& iterable, const T& value) {
    std::fill(iterable.begin(), iterable.end(), value);
  }

  template<
    typename Iterable,
    typename F,
    typename T = decltype(*std::declval<Iterable>().begin()), // TODO any simpler?
    typename R = std::invoke_result_t<F, T>
  >
  constexpr auto transform(const Iterable& iterable, const F& f) -> std::vector<R> {
    std::vector<R> result(iterable.size());
    std::transform(std::cbegin(iterable), std::cend(iterable), result.begin(), f);
    return result;
  }

  template<std::integral T>
  bool testAllBits(const T& bits, const T& mask) {
    return (bits & mask) == mask;
  }

  template<typename Iterable>
  constexpr auto iterableToVector(const Iterable& iterable) {
    return std::vector(iterable.begin(), iterable.end());
  }

} // namespace utl