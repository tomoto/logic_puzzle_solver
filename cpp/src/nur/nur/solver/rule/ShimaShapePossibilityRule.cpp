#include "ShimaShapePossibilityRule.h"

namespace nur {
  bool ShimaShapePossibilityRule::apply(Board& b) const {
    return evalAllOr(b.activeShimaList(), [&](Shima& shima) {
      return shima.dirty && processShima(b, shima);
      });
  }

  bool ShimaShapePossibilityRule::processShima(Board& b, Shima& shima) const {
    if (shima.number - shima.confirmedArea > 4) {
      return false; // 時期尚早
    }

    Array2D<int> count(shima.width, shima.height);

    ShimaShapeEnumerator e;
    e.enumerate(shima);

    for (auto& possibleShape : e.result) {
      int i = 0;
      for (auto& p : possibleShape.potential.data) {
        if (p > 0) {
          ++count.data[i];
        }
        ++i;
      }
    }

    bool changed = false;
    int maxCount = e.result.size();
    for (auto& y : range(0, shima.height)) {
      for (auto& x : range(0, shima.width)) {
        int n = count.at(x, y);
        Point p = shima.leftTop.add({ x, y });
        if (n == maxCount) {
          changed |= b.confirmShima(p, shima.id);
        } else if (n == 0) {
          changed |= shima.reject(p);
        }
      }
    }
    return changed;
  }

} // namespace nur
