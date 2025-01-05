#include "Formatter.h"
#include "utl/Array2DUtils.h"
#include "utl/StrUtils.h"
#include "utl/Ansi.h"

namespace nur {
  string Formatter::formatData(Board& b) {
    using namespace Array2DUtils;

    string result;

    result += "State:\n"
      + join(alignRight(transform(
        b.cells,
        [](CellState cell) { return std::to_string(cell.value); }\
      )), " ") + '\n';

    for (auto& shima : b.activeShimaList()) {
      result += "Shima " + std::to_string(shima.id) + "@" + shima.center.toString() + ":\n"
        + join(alignRight(transform(
          shima.potential,
          [](int value) { return std::to_string(value); } \
        )), " ") + '\n';
    }

    return result;
  }

  string Formatter::formatPretty(Board& b) {
    string result;

    for (auto y : range(0, b.height)) {
      for (auto x : range(0, b.width)) {
        Point p = { x, y };
        CellState cell = b.cellAt(p);
        // if (b.assumedFlags.at(x, y)) {
        //   result += Ansi::Red;
        // }
        if (cell.open()) {
          result += " .";
        } else if (cell.kabe()) {
          result += "\u2588\u2588";
        } else if (cell.confirmedShima()) {
          auto& shima = b.shima(cell.shimaId());
          if (shima.potentialAt(p) == shima.number) {
            result += StrUtils::alignRight(std::to_string(shima.number), 2);
          } else {
            result += " X";
          }
        } else if (cell.reservedShima()) {
          result += " ?";
        }
        // if (b.assumedFlags.at(x, y)) {
        //   result += Ansi::Reset;
        // }
      }
      result += "\n";
    }
    return result;
  }

} // namespace nur