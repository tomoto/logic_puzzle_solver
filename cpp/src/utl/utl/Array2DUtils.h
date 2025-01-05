#pragma once

#include "Base.h"
#include "Array2D.h"
#include "StrUtils.h"
#include "Utils.h"

namespace utl::Array2DUtils {
  inline vector<int> columnWidths(const Array2D<string>& a) {
    vector<int> result(a.width, 0);
    for (auto x : range(0, a.width)) {
      for (auto y : range(0, a.height)) {
        result[x] = std::max(result[x], int(a.at(x, y).size()));
      }
    }
    return result;
  }

  inline Array2D<string> alignRight(const Array2D<string>& a) {
    Array2D<string> result(a.width, a.height);
    auto widths = columnWidths(a);
    for (auto x : range(0, a.width)) {
      for (auto y : range(0, a.height)) {
        result.at(x, y) = StrUtils::alignRight(a.at(x, y), widths[x]);
      }
    }
    return result;
  }

  template<
    typename T,
    typename F,
    typename R = std::invoke_result_t<F, T>
  >
  Array2D<R> transform(const Array2D<T>& a, const F& f) {
    Array2D<R> result(a.width, a.height);
    for (auto i : range(0, a.data.size())) {
      result.data[i] = f(a.data[i]);
    }
    return result;
  }

  inline string join(
    const Array2D<string>& a,
    const string& columnSeparator = ",",
    const string& rowSeparator = "\n")
  {
    vector<string> rows;
    for (auto y : range(0, a.height)) {
      auto r = a.row(y);
      rows.push_back(StrUtils::join(r, columnSeparator));
    }
    return StrUtils::join(rows, rowSeparator);
  }

}