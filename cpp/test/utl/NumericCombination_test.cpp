#include "utl/NumericCombination.h"

#include <gtest/gtest.h>

using namespace utl;

TEST(utl_NumericCombination, basic) {
  NumericCombination a(5, 3);
  EXPECT_EQ(a.current, vector<int>({ 0, 1, 2 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 0, 1, 3 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 0, 1, 4 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 0, 2, 3 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 0, 2, 4 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 0, 3, 4 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 1, 2, 3 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 1, 2, 4 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 1, 3, 4 }));
  EXPECT_TRUE(a.next());
  EXPECT_EQ(a.current, vector<int>({ 2, 3, 4 }));
  EXPECT_FALSE(a.next());
}
