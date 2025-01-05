#pragma once

// #include "cmn/data/Rule.h"
#include "sli/data/Board.h"
#include "CellRule.h"

#include <functional>

namespace sli {

  template <CellRule R>
  struct CellRuleRunner {
    const R rule = {};

    bool apply(Board& b) {
      auto [xrange, yrange] = Helper::cellRangeFor(b, R::cellType, R::skipOutside);

      bool changed = false;
      for (int y : yrange) {
        if constexpr (R::skipUnmodifiedCells) {
          if (b.dirtyAtLine(y)) {
            for (int x : xrange) {
              changed |= b.dirtyAt(x, y) && rule.apply(b, x, y);
            }
          }
        } else {
          for (int x : xrange) {
            changed |= rule.apply(b, x, y);
          }
        }
      }
      return changed;
    }
  };

} // namespace sil
