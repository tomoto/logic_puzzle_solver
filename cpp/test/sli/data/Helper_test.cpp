#include "sli/data/Helper.h"
#include "sli/data/Board.h"

#include <gtest/gtest.h>

using namespace sli::Helper;

TEST(sli_Helper, cellTypeAt) {
  EXPECT_EQ(cellTypeAt(2, 2), CellType::number);
  EXPECT_EQ(cellTypeAt(1, 1), CellType::dot);
  EXPECT_EQ(cellTypeAt(2, 1), CellType::hline);
  EXPECT_EQ(cellTypeAt(1, 2), CellType::vline);
}

TEST(sli_Helper, numberIndex) {
  EXPECT_EQ(numberIndex(2, 2, 5), 4);
}

TEST(sli_Helper, cellRangeFor) {
  Board b(2, 1);
  auto ranges = cellRangeFor(b, CellType::number, false);
  auto [first, second] = ranges;
  EXPECT_EQ(vector(first.begin(), first.end()), vector({ 0, 2, 4, 6 }));
}
