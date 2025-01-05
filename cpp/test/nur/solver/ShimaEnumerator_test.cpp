#include "nur/data/ShimaShapeEnumerator.h"

#include <gtest/gtest.h>

using namespace nur;

TEST(nur_ShimaEnumerator, basic) {
  ShimaShapeEnumerator e;
  Shima shima(1, 3, { 2, 2 }, 5, 5);
  shima.confirm({ 2, 2 });
  e.enumerate(shima);
  EXPECT_EQ(e.result.size(), 18) << e.toString();
}

TEST(nur_ShimaEnumerator, partiallyRejected) {
  ShimaShapeEnumerator e;
  Shima shima(1, 3, { 2, 2 }, 5, 5);
  shima.confirm({ 2, 2 });
  shima.reject({ 1, 2 });
  e.enumerate(shima);
  EXPECT_EQ(e.result.size(), 12) << e.toString();
}

TEST(nur_ShimaEnumerator, partiallyConfirmed) {
  ShimaShapeEnumerator e;
  Shima shima(1, 3, { 2, 2 }, 5, 5);
  shima.confirm({ 2, 2 });
  shima.confirm({ 1, 2 });
  e.enumerate(shima);
  EXPECT_EQ(e.result.size(), 6) << e.toString();
}

TEST(nur_ShimaEnumerator, hookedShape) {
  ShimaShapeEnumerator e;
  Shima shima(1, 5, { 0, 0 }, 2, 3);
  shima.confirm({ 0, 0 });
  e.enumerate(shima);
  EXPECT_EQ(e.result.size(), 5) << e.toString();
}

TEST(nur_ShimaEnumerator, partiallyDisjointed1) {
  ShimaShapeEnumerator e;
  Shima shima(1, 4, { 0, 0 }, 2, 3);
  shima.confirm({ 0, 0 });
  shima.confirm({ 0, 2 });
  e.enumerate(shima);
  EXPECT_EQ(e.result.size(), 3) << e.toString();
}

TEST(nur_ShimaEnumerator, partiallyDisjointed2) {
  ShimaShapeEnumerator e;
  Shima shima(1, 4, { 3, 3 }, 7, 7);
  shima.confirm({ 3, 3 });
  shima.confirm({ 3, 5 });
  e.enumerate(shima);
  EXPECT_EQ(e.result.size(), 8) << e.toString();
}
