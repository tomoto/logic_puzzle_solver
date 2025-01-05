#include "sli/solver/rule/CellRuleRunner.h"
#include "sli/solver/rule/NumberCornerRule.h"
#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"

#include <gtest/gtest.h>

using namespace sli;

TEST(sli_NumberCornerRule, empty_noSmallLoop1) {
  auto b = Loader::loadFull({
    "+ + +",
    "  |  ",
    "+-+ +",
    "     ",
    "+ + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+x+ +\n"
    "x |  \n"
    "+-+ +\n"
    "     \n"
    "+ + +\n"
  );
}

TEST(sli_NumberCornerRule, empty_noSmallLoop2) {
  auto b = Loader::loadFull({
    "+ + +",
    "     ",
    "+-+ +",
    "  |  ",
    "+ + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + +\n"
    "     \n"
    "+-+ +\n"
    "x |  \n"
    "+x+ +\n"
  );
}


TEST(sli_NumberCornerRule, empty_noSmallLoop3) {
  auto b = Loader::loadFull({
    "+ +x+",
    "     ",
    "+ + +",
    "x |  ",
    "+ + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+x+x+\n"
    "x x  \n"
    "+x+ +\n"
    "x |  \n"
    "+ + +\n"
  );
}

TEST(sli_NumberCornerRule, one) {
  auto b = Loader::loadFull({
    "+ + +",
    "   1 ",
    "+ + +",
    "     ",
    "+ + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ +x+\n"
    "   1x\n"
    "+ + +\n"
    "     \n"
    "+ + +\n"
  );
}

TEST(sli_NumberCornerRule, three_rule1) {
  auto b = Loader::loadFull({
    "+ + +",
    "     ",
    "+ + +",
    "   3 ",
    "+ + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + +\n"
    "     \n"
    "+ + +\n"
    "   3|\n"
    "+ +-+\n"
  );
}

TEST(sli_NumberCornerRule, three_rule2) {
  auto b = Loader::loadFull({
    "+ + +",
    "  |  ",
    "+-+ +",
    "   3 ",
    "+ + +",
    "     ",
    "+ + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_THROW(r.apply(b), HatanException);
}

TEST(sli_NumberCornerRule, three_rule3) {
  auto b = Loader::loadFull({
    "+ + +",
    "  |  ",
    "+ + +",
    "   3 ",
    "+ + +",
    "     ",
    "+ + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + +\n"
    "  |  \n"
    "+x+ +\n"
    "   3|\n"
    "+ +-+\n"
    "     \n"
    "+ + +\n"
  );
}

TEST(sli_NumberCornerRule, three_rule1_transitive) {
  auto b = Loader::loadFull({
    "+ + + +",
    "       ",
    "+ + + +",
    "   3   ",
    "+ + + +",
    "     2 ",
    "+ + + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + + +\n"
    "       \n"
    "+ + + +\n"
    "   3|  \n"
    "+ +-+ +\n"
    "     2|\n"
    "+ + +-+\n"
  );
}

TEST(sli_NumberCornerRule, three_rule2_transitive) {
  auto b = Loader::loadFull({
    "+ + + +",
    "  |    ",
    "+-+ + +",
    "   2   ",
    "+ + + +",
    "     3 ",
    "+ + + +",
    "       ",
    "+ + + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_THROW(r.apply(b), HatanException);
}

TEST(sli_NumberCornerRule, three_rule3_transitive) {
  auto b = Loader::loadFull({
    "+ + + +",
    "  |    ",
    "+ + + +",
    "   2   ",
    "+ + + +",
    "     3 ",
    "+ + + +",
    "       ",
    "+ + + +",
    });
  CellRuleRunner<NumberCornerRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + + +\n"
    "  |    \n"
    "+x+ + +\n"
    "   2   \n"
    "+ + + +\n"
    "     3|\n"
    "+ + +-+\n"
    "       \n"
    "+ + + +\n"
  );
}