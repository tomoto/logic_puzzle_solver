#include "sli/solver/rule/CellRuleRunner.h"
#include "sli/solver/rule/ThreePairInitialRule.h"
#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"

#include <gtest/gtest.h>

using namespace sli;

TEST(sli_ThreePairInitialRule, adjacent_noSmallLoop) {
  auto b = Loader::loadNumbers({
    "-3-",
    "-3-",
    });
  CellRuleRunner<ThreePairInitialRule<true>> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ +-+ +\n"
    "   3   \n"
    "+x+-+x+\n"
    "   3   \n"
    "+ +-+ +\n"
  );

  EXPECT_EQ(r.apply(b), false);
}

TEST(sli_ThreePairInitialRule, adjacent_withSmallLoop) {
  auto b = Loader::loadNumbers({
    "-3--",
    "-3--",
    });
  CellRuleRunner<ThreePairInitialRule<false>> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ +-+ + +\n"
    "   3     \n"
    "+x+ +x+ +\n"
    "   3     \n"
    "+ +-+ + +\n"
  );

  EXPECT_EQ(r.apply(b), false);
}

TEST(sli_ThreePairInitialRule, diagonal) {
  auto b = Loader::loadNumbers({
    "-3",
    "3-",
    });
  CellRuleRunner<ThreePairInitialRule<true>> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ +-+\n"
    "   3|\n"
    "+ + +\n"
    "|3   \n"
    "+-+ +\n"
  );

  EXPECT_EQ(r.apply(b), false);
}

TEST(sli_ThreePairInitialRule, diagonal_transitive) {
  auto b = Loader::loadNumbers({
    "--3",
    "-2-",
    "3--",
    });
  CellRuleRunner<ThreePairInitialRule<false>> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + +-+\n"
    "     3|\n"
    "+ + + +\n"
    "   2   \n"
    "+ + + +\n"
    "|3     \n"
    "+-+ + +\n"
  );

  EXPECT_EQ(r.apply(b), false);
}
