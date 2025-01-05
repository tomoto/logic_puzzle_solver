#pragma once

#include "Base.h"
#include "Range.h"

#include <concepts>

namespace utl {

  /// n個の整数からr個を選ぶ組み合わせを列挙する
  struct NumericCombination {
    int _n;
    vector<int> current;

    NumericCombination(int n, int r) : _n(n), current(r) {
      for (int& i : range(0, r)) {
        current[i] = i;
      }
    }

    bool next() {
      int r = current.size();
      int i;
      for (i = r - 1; i >= 0; --i) {
        if (current[i] < _n - r + i) {
          break;
        }
      }

      if (i < 0) {
        return false; // 全ての組み合わせを列挙済み
      }

      ++current[i];
      for (int j = i + 1; j < r; j++) {
        current[j] = current[j - 1] + 1;
      }

      return true;
    }

  };

} // namespace utl
