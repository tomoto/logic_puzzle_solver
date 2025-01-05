#include "sli/solver/rule/CellRuleRunner.h"
#include "sli/solver/rule/BasicNumberRule.h"
#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"

#include <gtest/gtest.h>

using namespace sli;

TEST(sli_BasicNumberRule, basic) {
  auto b = Loader::loadFull({
    "+ + +",
    " 2|  ",
    "+ + +",
    " 3 0 ",
    "+ + +",
    });
  CellRuleRunner<BasicNumberRule> r;

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + +\n"
    " 2|  \n"
    "+ +x+\n"
    " 3x0x\n"
    "+ +x+\n"
  );

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+ + +\n"
    " 2|  \n"
    "+-+x+\n"
    "|3x0x\n"
    "+-+x+\n"
  );

  EXPECT_EQ(r.apply(b), true);
  EXPECT_EQ(Formatter::formatFull(b),
    "+x+ +\n"
    "x2|  \n"
    "+-+x+\n"
    "|3x0x\n"
    "+-+x+\n"
  );

  EXPECT_EQ(r.apply(b), false);
}
