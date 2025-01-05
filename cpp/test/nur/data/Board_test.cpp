#include "nur/data/Board.h"
#include "nur/serializer/Loader.h"
#include "nur/serializer/Formatter.h"

#include <gtest/gtest.h>

using namespace nur;

TEST(nur_Board, load) {
  auto b = Loader::load({
    "3 0 0",
    "0 0 0",
    "0 2 0",
    });
  EXPECT_EQ(Formatter::formatData(b),
    "State:\n"
    "1 0 0\n"
    "0 0 0\n"
    "0 2 0\n"
    "Shima 1@(0,0):\n"
    "3 2 1\n"
    "2 0 0\n"
    "0 0 0\n"
    "Shima 2@(1,2):\n"
    "0 1 0\n"
    "1 2 1\n"
  );
}
