#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"
#include "utl/StrUtils.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace sli;
using namespace utl::StrUtils;

TEST(sli_Loader, loadNumbers) {
  vector<string> lines = {
    "1-3",
    "-0-",
  };
  auto b = Loader::loadNumbers(lines);
  EXPECT_EQ(b.logicalWidth, 3);
  EXPECT_EQ(b.logicalHeight, 2);
  EXPECT_EQ(b.numbers.at(1, 1), 1);
  EXPECT_EQ(b.numberAt(2, 2), 1);

  std::stringstream ss;
  Formatter::formatNumbers(ss, b);
  EXPECT_EQ(ss.str(), join(lines, "\n") + "\n");

  EXPECT_TRUE(std::ranges::equal(b.numbers.data, std::array<int, 20>{
    -1, -1, -1, -1, -1,
      -1, 1, -1, 3, -1,
      -1, -1, 0, -1, -1,
      -1, -1, -1, -1, -1,
  })
  );
}

TEST(sli_Loader, loadFull) {
  vector<string> lines = {
    "+-+-+-+",
    "|2   3|",
    "+ + +-+",
    "x  2| x",
    "+ +-+ +",
  };
  auto b = Loader::loadFull(lines);
  EXPECT_EQ(b.logicalWidth, 3);
  EXPECT_EQ(b.logicalHeight, 2);
  EXPECT_EQ(b.numberAt(2, 2), 2);

  std::stringstream ss;
  Formatter::formatFull(ss, b);
  EXPECT_EQ(ss.str(), join(lines, "\n") + "\n");
}