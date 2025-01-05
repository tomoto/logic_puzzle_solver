#pragma once

#include "nur/data/Board.h"
#include "nur/data/ShimaShapeEnumerator.h"

namespace nur {
  /// シマのあり得る形を列挙して行なう処理
  /// TODO
  /// - 遅い
  /// - シマ自身の状態なので、シマのロジックであるべき?
  struct ShimaShapePossibilityRule {
    bool apply(Board& b) const;

  private:
    bool processShima(Board& b, Shima& shima) const;
  };

} // namespace nur
