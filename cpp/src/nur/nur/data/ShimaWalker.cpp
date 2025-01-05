#include "ShimaWalker.h"
#include "Shima.h"

namespace nur {

  ShimaWalker::ShimaWalker(Shima& shima0, const Point& start) :
    shima(shima0),
    walked(shima0.width, shima0.height),
    locallyWalked(shima0.width, shima0.height)
  {
    int leg = shima.potentialAt(start);
    walk(start, leg);
  }

  void ShimaWalker::walk(const Point& p, int leg) {
    std::fill(RANGE(locallyWalked.data), 0);
    walkInternal(p, leg);

    int n = shima.potentialAt(p);
    for (auto& d : Directions::adjacents) {
      auto q = p.add(d);
      if (shima.potentialAt(q) > n) {
        walk(q, leg);
      }
    }
  }

  void ShimaWalker::walkInternal(const Point& p, int leg) {
    if (leg == 0) return;

    auto pp = p.sub(shima.leftTop);
    if (shima.potentialAt(p) <= 0 || locallyWalked.at(pp.x, pp.y) >= leg) {
      return;
    }

    locallyWalked.at(pp.x, pp.y) = leg;
    walked.at(pp.x, pp.y) = true;

    for (auto& d : Directions::adjacents) {
      walkInternal(p.add(d), leg - 1);
    }
  }

  bool ShimaWalker::walkedAt(const Point& p) {
    auto pp = p.sub(shima.leftTop);
    return walked.at(pp.x, pp.y);
  }

} // namespace nur