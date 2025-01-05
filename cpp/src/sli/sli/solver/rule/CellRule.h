#pragma once

#include "sli/data/Board.h"

#include <concepts>

namespace sli {

template <typename R>
concept CellRule = requires(const R & r, Board & b, int x, int y) {
  { R::cellType } -> std::convertible_to<CellType>;
  { R::skipOutside } -> std::convertible_to<bool>;
  { R::skipUnmodifiedCells } -> std::convertible_to<bool>;
  { r.apply(b, x, y) } -> std::convertible_to<bool>;
};

}