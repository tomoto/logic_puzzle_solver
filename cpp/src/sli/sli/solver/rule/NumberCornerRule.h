#pragma once

#include "CellRule.h"
#include "utl/Point.h"

namespace sli {

  struct NumberCornerRule {
    static constexpr CellType cellType = CellType::number;
    static constexpr bool skipOutside = true;
    static constexpr bool skipUnmodifiedCells = true;

    bool apply(Board& b, int x, int y) const;
  };

  static_assert(CellRule<NumberCornerRule>);

} // namespace sli