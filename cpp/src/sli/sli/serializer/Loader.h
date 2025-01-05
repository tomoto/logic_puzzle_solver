#pragma once

#include "sli/data/Base.h"
#include "sli/data/Board.h"

namespace sli {

struct Loader {
  static Board loadNumbers(vector<string> lines);
  static Board loadFull(vector<string> lines);
};

}