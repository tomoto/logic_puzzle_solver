#pragma once

#include "nur/data/Board.h"

namespace nur {

  /// シマポテンシャルの重なりにより確定するマスの処理
  struct SPOverwrapRule {
    bool apply(Board& b) const;
  };

} //
