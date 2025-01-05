#pragma once

#include "Base.h"
#include "NumericIterator.h"

#define RANGE(iterable) begin(iterable), end(iterable)

namespace utl {

  template <typename T>
  struct Range {
    NumericIterator<T> _begin;
    NumericIterator<T> _end;

    constexpr Range(T begin, T end, T step) :
      _begin(begin, step),
      _end(adjustEnd(begin, end, step), 0) {}

    const auto begin() const { return _begin; }
    const auto end() const { return _end; }

  private:
    static int adjustEnd(T begin, T end, T step) {
      auto r = (end - begin) % step;
      return (r < 0) ? begin : (r != 0) ? end + step - r : end;
    }
  };

  inline constexpr Range<int> range(int begin, int end, int step = 1) {
    return { begin, end, step };
  }

} // namespace utl