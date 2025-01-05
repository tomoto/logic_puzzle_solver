#include "sli/solver/rule/CellRuleRunner.h"
#include "sli/solver/rule/DotRule.h"
#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"

#include <gtest/gtest.h>

using namespace sli;

TEST(sli_DotRule, basic) {
  auto b = Loader::loadFull({
    "+ +x+ + +",
    "  x |    ",
    "+ + +-+ +",
    "         ",
    "+ + + + +",
    });
  CellRuleRunner<DotRule> r;
  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+x+x+-+ +\n"
    "  x |    \n"
    "+ +x+-+ +\n"
    "    x    \n"
    "+ + + + +\n"
  );

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+x+x+-+ +\n"
    "x x |    \n"
    "+ +x+-+ +\n"
    "    x    \n"
    "+ + + + +\n"
  );

  EXPECT_EQ(r.apply(b), false);
}
