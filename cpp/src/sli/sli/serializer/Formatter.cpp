#include "Formatter.h"
#include "utl/Ansi.h"

#include <functional>

namespace sli {

  std::ostream& Formatter::formatNumbers(std::ostream& os, Board& b) {
    for (int y : range(1, b.logicalHeight + 1)) {
      for (int x : range(1, b.logicalWidth + 1)) {
        os << NumberCell::itoc(b.numbers.at(x, y), '-');
      }
      os << '\n';
    }
    return os;
  }

  std::ostream& Formatter::formatFull(
    std::ostream& os,
    Board& b,
    const Formatter::Options& opt)
  {
    using namespace utl::Ansi;

    auto hline = opt.pretty ? "\u2501" : "-";
    auto vline = opt.pretty ? "\u2503" : "|";
    auto dot = opt.pretty ? bold("\u00b7") : "+";

    auto formatLine = [&](int x, int y, const char* symbol) {
      auto& line = b.lineAt(x, y);
      if (opt.pretty) {
        if (line.exists) {
          os << bold((line.assumed ? yellow(symbol) : green(symbol)));
        } else if (line.forbidden) {
          os << dim(line.assumed ? yellow("x") : red("x"));
        } else {
          os << " ";
        }
      } else {
        os << (line.exists ? symbol : line.forbidden ? "x" : " ");
      }};

    auto formatCell = [&](int x, int y) {
      switch (Helper::cellTypeAt(x, y)) {
      case CellType::number:
        os << NumberCell::itoc(b.numberAt(x, y), ' ');
        break;
      case CellType::dot:
        os << dot;
        break;
      case CellType::hline:
        formatLine(x, y, hline);
        break;
      case CellType::vline:
        formatLine(x, y, vline);
        break;
      }};

    for (int y : range(1, b.physicalHeight - 1)) {
      for (int x : range(1, b.physicalWidth - 1)) {
        if (b.dirtyAt(x, y) && opt.pretty && opt.highlightDirty) {
          os << BgBlue;
          formatCell(x, y);
          os << Reset;
        } else {
          formatCell(x, y);
        }
      }
      os << '\n';
    }
    return os;
  }

} // namespace sli