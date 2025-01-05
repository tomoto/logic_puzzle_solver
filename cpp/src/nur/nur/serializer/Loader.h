#pragma once

#include "nur/data/Board.h"

namespace nur {

  struct Loader {
    static Board load(vector<string> lines);
  };
}