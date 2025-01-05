#include "sli/solver/rule/CellRuleRunner.h"
#include "sli/solver/rule/IncomingCornerRule.h"
#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"

#include <gtest/gtest.h>

using namespace sli;

TEST(sli_IncomingCornerRule, empty) {
  auto b = Loader::loadFull({
    "+ + +",
    "     ",
    "+-+x+",
    "  |  ",
    "+ + +",
    });
  CellRuleRunner<IncomingCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + +\n"
    "  x  \n"
    "+-+x+\n"
    "  |  \n"
    "+ + +\n"
  );
}

TEST(sli_IncomingCornerRule, one) {
  auto b = Loader::loadFull({
    "+ + +",
    "  |  ",
    "+ + +",
    "x1   ",
    "+x+ +",
    });
  CellRuleRunner<IncomingCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + +\n"
    "  |  \n"
    "+ +x+\n"
    "x1   \n"
    "+x+ +\n"
  );
}

TEST(sli_IncomingCornerRule, three) {
  auto b = Loader::loadFull({
    "+ + +",
    "   3 ",
    "+ +-+",
    "|3   ",
    "+-+ +",
    });
  CellRuleRunner<IncomingCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ +-+\n"
    "  x3|\n"
    "+ +-+\n"
    "|3   \n"
    "+-+ +\n"
  );
}

TEST(sli_IncomingCornerRule, two) {
  auto b = Loader::loadFull({
    "+ + + +",
    "       ",
    "+ + +-+",
    "x x2   ",
    "+ + + +",
    " 2     ",
    "+ + + +",
    "       ",
    "+ + + +",
    });
  CellRuleRunner<IncomingCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + + +\n"
    "    x  \n"
    "+ + +-+\n"
    "x x2   \n"
    "+ +-+ +\n"
    " 2     \n"
    "+ + + +\n"
    "|      \n"
    "+-+ + +\n"
  );
}
