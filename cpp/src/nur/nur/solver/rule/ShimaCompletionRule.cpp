#include "ShimaCompletionRule.h"

namespace nur {

  bool ShimaCompletionRule::apply(Board& b) const {
    bool changed = false;

    for (auto& shima : b.activeShimaList()) {
      if (shima.dirty && !shima.complete() && shima.possibleArea == shima.number) {
        changed |= confirmAllPossible(b, shima);
      }
    }

    return changed;
  }

  bool ShimaCompletionRule::confirmAllPossible(Board& b, Shima& shima) const {
    bool changed = false;
    for (auto y : shima.yrange()) {
      for (auto x : shima.xrange()) {
        Point p = { x, y };
        if (shima.potentialAt(p) > 0) {
          changed |= b.confirmShima(p, shima.id);
        }
      }
    }
    return changed;
  }

} // namespace nur