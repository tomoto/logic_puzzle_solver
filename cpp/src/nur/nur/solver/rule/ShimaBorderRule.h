#pragma once

#include "nur/data/Board.h"

namespace nur {

  /// シマの領地の境界となるカベを確定するルール
  struct ShimaBorderRule {
    bool apply(Board& b) const;
  };

}
