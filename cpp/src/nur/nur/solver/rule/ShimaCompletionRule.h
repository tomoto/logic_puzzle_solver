#pragma once

#include "nur/data/Board.h"

namespace nur {

  /// 消去法で全領地が決まったシマに対する確定処理
  struct ShimaCompletionRule {
    bool apply(Board& b) const;

  private:
    bool confirmAllPossible(Board& b, Shima& shima) const;
  };

}
