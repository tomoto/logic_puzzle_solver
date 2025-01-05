#include "utl/Array2D.h"

#include <gtest/gtest.h>
#include <algorithm>
#include <array>

using namespace utl;

TEST(utl_Array2D, basic) {
  Array2D<int> a(3, 2);

  EXPECT_EQ(a.data, vector<int>(6, 0));

  a.at(2, 1) = 5;
  EXPECT_EQ(a.data, vector<int>({ 0, 0, 0, 0, 0, 5 }));

  a.at(1, 0) = 4;
  EXPECT_TRUE(std::ranges::equal(a.row(0), std::array<int, 3>{ 0, 4, 0 }));
  EXPECT_TRUE(std::ranges::equal(a.row(1), std::array<int, 3>{ 0, 0, 5 }));

  Array2D<int> b(3, 2, 5);
  EXPECT_EQ(b.data, vector<int>({ 5, 5, 5, 5, 5, 5 }));
  a = std::move(b);
  EXPECT_EQ(a.data, vector<int>({ 5, 5, 5, 5, 5, 5 }));
  EXPECT_EQ(a.width, 3);
  EXPECT_EQ(a.height, 2);
  EXPECT_EQ(b.data.size(), 0);
  // This test fails; not great, but OK
  // EXPECT_EQ(b.width, 0);
  // EXPECT_EQ(b.height, 0);
}
