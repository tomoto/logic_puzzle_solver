#include "Loader.h"
#include "cmn/data/LoadingException.h"

namespace sli {

  static void check(bool condition, const string& message) {
    if (!condition) {
      throw cmn::LoadingException(message);
    }
  }

  Board Loader::loadNumbers(vector<string> lines) {
    int h = lines.size();
    check(h > 0, "The height of the board should be positive");
    int w = lines[0].size();
    check(w > 0, "The width of the board should be positive");

    Board b(w, h);
    for (int y : range(0, h)) {
      check(lines[y].size() == w, "The width of the board should be consistent");
      for (int x : range(0, w)) {
        char c = lines[y][x];
        b.numbers.at(x + 1, y + 1) = NumberCell::ctoi(c);
      }
    }

    return std::move(b);
  }

  Board Loader::loadFull(vector<string> lines) {
    int physicalHeight = lines.size();
    check((physicalHeight & 1) && (physicalHeight >= 3),
      "The height of the board should be odd and at least 3");
    int physicalWidth = lines[0].size();
    check((physicalWidth & 1) && (physicalWidth >= 3),
      "The width of the board should be odd and at least 3");
    int logicalHeight = (physicalHeight - 1) / 2;
    int logicalWidth = (physicalWidth - 1) / 2;

    Board b(logicalWidth, logicalHeight);
    for (int y : range(1, physicalHeight + 1)) {
      for (int x : range(1, physicalWidth + 1)) {
        char c = lines[y - 1][x - 1];
        switch (Helper::cellTypeAt(x, y)) {
        case CellType::number:
          check((c >= '0' && c <= '3') || c == ' ',
            "The number cell should be 0-3 or space");
          b.numberAt(x, y) = NumberCell::ctoi(c);
          break;
        case CellType::dot:
          check(c == '+', "The dot cell should be +");
          break;
        case CellType::hline:
        case CellType::vline:
          check(c == '-' || c == '|' || c == 'x' || c == ' ',
            "The line cell should be -, |, x, or space");
          switch (c) {
          case '-':
          case '|':
            b.setExistsAt(x, y);
            break;
          case 'x':
            b.setForbiddenAt(x, y);
            break;
          }
          break;
        }
      }
    }

    return std::move(b);
  }

}