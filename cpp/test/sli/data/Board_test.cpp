#include "sli/data/Board.h"

#include <gtest/gtest.h>

using namespace sli;

TEST(sli_Board, basic) {
  Board b(3, 2);
  EXPECT_EQ(b.physicalWidth, 9);
  EXPECT_EQ(b.physicalHeight, 7);

  auto& ss = b.surroundingStatsAt(2, 2);
  EXPECT_EQ(ss.exists, 0);
  EXPECT_EQ(ss.forbidden, 0);

  b.setExistsAt(2, 1);
  EXPECT_TRUE(b.lineAt(2, 1).exists);

  b.setExistsAt(3, 2);

  b.setForbiddenAt(1, 2);
  EXPECT_TRUE(b.lineAt(1, 2).forbidden);

  EXPECT_EQ(ss.exists, 2);
  EXPECT_EQ(ss.forbidden, 1);
}
