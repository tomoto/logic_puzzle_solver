#pragma once

#include "Base.h"

#include <iterator>

namespace utl {

  template <typename T>
  struct NumericIterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using difference_type = decltype(T() - T());
    using pointer = T*;
    using reference = T&;

    T value;
    T step;

    constexpr NumericIterator(T value, T step) : value(value), step(step) {}

    constexpr NumericIterator& operator++() {
      value += step;
      return *this;
    }

    constexpr NumericIterator operator++(int) {
      NumericIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    constexpr bool operator==(const NumericIterator& rhs) const {
      return value == rhs.value;
    }

    constexpr bool operator!=(const NumericIterator& rhs) const {
      return !(*this == rhs);
    }

    constexpr reference operator*() { return value; }

    constexpr value_type operator*() const { return value; }
  };

} // namespace utl