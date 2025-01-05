#include "sli/solver/rule/CellRuleRunner.h"
#include "sli/solver/rule/ThreePairInitialRule.h"
#include "sli/solver/rule/BasicNumberRule.h"
#include "sli/solver/rule/DotRule.h"
#include "sli/solver/rule/NumberCornerRule.h"
#include "sli/solver/rule/IncomingCornerRule.h"
#include "sli/serializer/Loader.h"
#include "sli/serializer/Formatter.h"
#include "utl/Utils.h"

#include <gtest/gtest.h>
#include <sstream>

using namespace sli;

TEST(sli_CellRuleCombination, basic) {
  auto b = Loader::loadNumbers({
    "--31",
    "-3--",
    "-3--",
    });


  {
    CellRuleRunner<ThreePairInitialRule<true>> r;
    EXPECT_EQ(r.apply(b), true);
    EXPECT_EQ(r.apply(b), false);
    EXPECT_EQ(Formatter::formatFull(b),
      "+ + +-+ +\n"
      "     3|1 \n"
      "+ +-+ + +\n"
      "  |3     \n"
      "+x+-+x+ +\n"
      "   3     \n"
      "+ +-+ + +\n"
    );
  }

  {
    CellRuleRunner<BasicNumberRule> r;
    EXPECT_EQ(r.apply(b), true);
    EXPECT_EQ(Formatter::formatFull(b),
      "+ + +-+x+\n"
      "     3|1x\n"
      "+ +-+ +x+\n"
      "  |3x    \n"
      "+x+-+x+ +\n"
      "   3     \n"
      "+ +-+ + +\n"
    );
  }

  {
    CellRuleRunner<DotRule> r;
    EXPECT_EQ(r.apply(b), true);
    EXPECT_EQ(r.apply(b), true);
    EXPECT_EQ(Formatter::formatFull(b),
      "+ + +-+x+\n"
      "  x  3|1x\n"
      "+x+-+ +x+\n"
      "  |3x   x\n"
      "+x+-+x+ +\n"
      "| x3|    \n"
      "+-+-+x+ +\n"
    );
  }

  {
    CellRuleRunner<NumberCornerRule> r;
    EXPECT_EQ(r.apply(b), true);
    EXPECT_EQ(Formatter::formatFull(b),
      "+ + +-+x+\n"
      "  x  3|1x\n"
      "+x+-+ +x+\n"
      "  |3x   x\n"
      "+x+-+x+x+\n"
      "| x3| x x\n"
      "+-+-+x+x+\n"
    );
  }

}
