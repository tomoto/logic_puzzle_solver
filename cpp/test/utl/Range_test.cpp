#include "utl/Range.h"
#include <utl/Utils.h>

#include <gtest/gtest.h>

using namespace utl;

TEST(utl_Range, basic) {
  NumericIterator<int> begin(1, 2);
  NumericIterator<int> end(7, 0);
  EXPECT_EQ(vector<int>(begin, end), vector({ 1, 3, 5 }));

  {
    vector<int> v;
    for (int i : range(1, 7)) {
      v.push_back(i);
    }
    EXPECT_EQ(v, vector({ 1, 2, 3, 4, 5, 6 }));
  }

  {
    auto r = range(1, 7, 2);
    EXPECT_EQ(vector(r.begin(), r.end()), vector({ 1, 3, 5 }));
  }

  {
    auto r = range(1, 6, 2);
    EXPECT_EQ(vector(r.begin(), r.end()), vector({ 1, 3, 5 }));
  }
}
