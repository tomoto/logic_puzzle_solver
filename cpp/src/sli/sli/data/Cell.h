#pragma once

#include "sli/data/Base.h"
#include "cmn/data/HatanException.h"

#include <array>

namespace sli {

  enum class CellType {
    number,
    vline,
    hline,
    dot,
  };

  struct NumberCell {
    static constexpr int empty = -1;

    static char itoc(int n, char emptyChar = ' ') {
      return (n == empty) ? emptyChar : ('0' + n);
    }

    static int ctoi(char c) {
      return (c >= '0' && c <= '3') ? (c - '0') : empty;
    }
  };

  struct LineCellInfo {
    uint8_t exists : 1 = false;
    uint8_t forbidden : 1 = false;
    uint8_t assumed : 1 = false;
    uint8_t fixed() const { return exists || forbidden; }
  };
}
