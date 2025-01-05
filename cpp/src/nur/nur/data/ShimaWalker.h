#pragma once

#include "nur/data/Base.h"
#include "utl/Point.h"
#include "utl/Array2D.h"

namespace nur {
  struct Shima;

  /// シマを渡り歩いて到達可能範囲を大まかに見積もる
  struct ShimaWalker {
    Shima& shima;
    Array2D<bool> walked;
    Array2D<int> locallyWalked;

    ShimaWalker(Shima& shima0, const Point& start);
    bool walkedAt(const Point& p);

  private:
    void walk(const Point& p, int leg);
    void walkInternal(const Point& p, int leg);
  };

} // namespace nur