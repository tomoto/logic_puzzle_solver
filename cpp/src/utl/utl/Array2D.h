#pragma once

#include <vector>
#include <span>
#include <cassert>

#include "Base.h"
#include "Range.h"

namespace utl {

  template <typename T>
  struct Array2D {
    int width;
    int height;
    vector<T> data;

    constexpr Array2D(int w, int h) :
      width(w), height(h), data(w* h) {}
    constexpr Array2D(int w, int h, const T& initialValue) :
      width(w), height(h), data(w* h, initialValue) {}

    constexpr decltype(auto) at(int x, int y) {
      validatePosition(x, y);
      return data[y * width + x];
    }

    constexpr decltype(auto) at(int x, int y) const {
      validatePosition(x, y);
      return data[y * width + x];
    }

    constexpr int index(int x, int y) const {
      validatePosition(x, y);
      return y * width + x;
    }

    constexpr std::span<const T> row(int y) const {
      assert(y >= 0 && y < height);
      return std::span<const T>(&data[y * width], width);
    }

  private:
    constexpr void validatePosition(int x, int y) const {
      assert(x >= 0 && x < width && y >= 0 && y < height);
    }
  };

} // namespace utl
