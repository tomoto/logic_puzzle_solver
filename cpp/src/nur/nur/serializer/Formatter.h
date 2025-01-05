#pragma once

#include "nur/data/Board.h"

namespace nur {

  struct Formatter {
    static string formatData(Board& board);
    static string formatPretty(Board& board);
  };

}