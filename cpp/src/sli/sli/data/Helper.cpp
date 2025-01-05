#include "Helper.h"
#include "Board.h"

namespace sli {

static struct {
  int xmin, ymin, step;
} cellTypeRangeBaseMap[] = {
  // NOTE: dependent to CellType order
  { 0, 0, 2 }, // number
  { 1, 0, 2 }, // vline
  { 0, 1, 2 }, // hline
  { 1, 1, 2 }, // dot
};

Helper::RangePair Helper::cellRangeFor(Board& b, CellType cellType, bool skipOutside) {
  using enum CellType;
  auto [xmin, ymin, step] = cellTypeRangeBaseMap[static_cast<int>(cellType)];
  int xmax = b.physicalWidth;
  int ymax = b.physicalHeight;
  if (skipOutside) {
    if (xmin == 0) {
      xmin += 2;
      xmax -= 1;
    }
    if (ymin == 0) {
      ymin += 2;
      ymax -= 1;
    }
  }

  return { range(xmin, xmax, step), range(ymin, ymax, step) };
}

}