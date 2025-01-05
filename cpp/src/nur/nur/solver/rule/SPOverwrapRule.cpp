#include "SPOverwrapRule.h"

#include <iostream>

namespace nur {

  bool SPOverwrapRule::apply(Board& b) const {
    Array2D<int> overwrap(b.width, b.height, 0);

    for (auto& shima : b.activeShimaList()) {
      for (auto& points : shima.possiblePointsByPotential) {
        for (auto& p : points) {
          // シマポテンシャルがある場合、始めてならシマIDを、2回目以降なら-1をセット
          auto& w = overwrap.at(p.x, p.y);
          w = (w == 0) ? shima.id : -1;
        }
      }
    }

    bool changed = false;

    for (auto y : range(0, b.height)) {
      for (auto x : range(0, b.width)) {
        auto cell = b.cellAt({ x, y });
        if (cell.confirmedShima() || cell.kabe()) continue;

        auto w = overwrap.at(x, y);
        if (w == 0) {
          // シマポテンシャルがないのでカベ確定
          changed |= b.confirmKabe({ x, y });
        } else if (w > 0) {
          // シマポテンシャルが1つしかない
          // reservedであればこのシマに確定
          if (b.cellAt({ x, y }).reservedShima()) {
            changed |= b.confirmShima({ x, y }, w);
          }
        }
      }
    }

    return changed;
  }

} //
