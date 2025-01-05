#include "ShimaBorderRule.h"

namespace nur {

  bool ShimaBorderRule::apply(Board& b) const {
    bool changed = false;

    for (auto& shima : b.activeShimaList()) {
      if (!shima.dirty) continue;

      for (auto& points : shima.confirmedPointsByPotential) {
        for (auto& p : points) {
          // 確定した白マスの周囲でポテンシャルがゼロに落ちる場合、そこはカベで確定
          for (auto d : Directions::adjacents) {
            Point q = p.add(d);
            if (shima.potentialAt(q) == 0) {
              changed |= b.confirmKabe(q);
            }
          }
        }
      }
    }

    return changed;
  }

}