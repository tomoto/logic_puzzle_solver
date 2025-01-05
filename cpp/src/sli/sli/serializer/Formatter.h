#pragma once

#include "sli/data/Base.h"
#include "sli/data/Board.h"
#include "utl/Utils.h"

#include <memory>
#include <ostream>

namespace sli {

struct Formatter {
  struct Options {
    bool pretty = false;
    bool highlightDirty = false;
  };

  static std::ostream& formatNumbers(std::ostream& os, Board& b);

  static string formatNumbers(Board& b) {
    return streamToString([&](std::ostream& os) { formatNumbers(os, b); });
  }

  static std::ostream& formatFull(std::ostream& os, Board& b, const Options& opt);

  static std::ostream& formatFull(std::ostream& os, Board& b) {
    return formatFull(os, b, {});
  }

  static string formatFull(Board& b, const Options& opt) {
    return streamToString([&](std::ostream& os) { formatFull(os, b, opt); });
  }

  static string formatFull(Board& b) {
    return formatFull(b, {});
  }
};

} // namespace sli